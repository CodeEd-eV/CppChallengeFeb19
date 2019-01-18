

#include "Network/Server.h"
#include "Game/ConnectFour.h"
#include "Game/Player.h"

#include <iostream>
#include <thread>


class ClientPlayer {
public:

    ClientPlayer(std::string nm, Connection&& con) : name(std::move(nm)), connection(std::move(con)) {

    }

    std::uint32_t chooseMove(ConnectFour& cf) {

        char mv[2] = {0};
        connection.receive(mv);
        return (std::uint32_t) std::atoi(mv);


    }

    void tellGameState(const LLGameInfo& gameInfo) {

        std::cout << "GO!" << std::endl;

    }

    const std::string& getName() const {

        return name;

    }

private:

    std::string name;
    Connection connection;

};

std::array<Player, 2> MakePlayers(ClientPlayer&& cp, std::uint32_t config) {

    switch (config) {
        case 0:
            return {MakeRandomAIPlayer(FIELD_STATE::RED, 0), //Here: Beginner is always red
                    Player(std::move(cp), FIELD_STATE::YELLOW, 1)};

        case 1:
            return {Player(std::move(cp), FIELD_STATE::RED, 0),
                    MakeRandomAIPlayer(FIELD_STATE::YELLOW, 1)};

    }

    throw std::exception(); //Can not happen!

}

RESULT_REASON Move(ConnectFour& cf, Player& player, Player& other) {

    player.tellGameState(cf, other, GAME_RESULT::CONTINUE, RESULT_REASON::REGULAR);
    if(auto mv = player.chooseMove(cf); mv != Player::TIMEOUT_MOVE) {

        return Insert(cf, player.getColor(), mv);

    }

    return RESULT_REASON::TIMEOUT;

}

int main() {

    auto server = Server(40596);
    server.init(10);

    std::uint32_t ConfigCounter = 0; //This is used to generate alternating game states, like who begins with which colour
    for (auto refConnection : server) {

        std::thread([connection = std::move(refConnection.get())] (std::uint32_t config) mutable {

            char nameBuffer[15] = {0};
            if(connection.receive(nameBuffer, sizeof(nameBuffer) - 1) != NET_RESULT::OK) {
                return;
            }

            auto player = MakePlayers(ClientPlayer(nameBuffer, std::move(connection)), config);

            ConnectFour field;
            bool GameOver = false;
            while(!GameOver) {
                for (int i = 0; i < 2; i++) {

                    try {

                        if (auto result = Move(field, player[i], player[(i + 1) % 2]); result !=
                                                                                       RESULT_REASON::REGULAR) {
                            auto winner =
                                    player[i].getColor() == FIELD_STATE::YELLOW ? GAME_RESULT::RED
                                                                                : GAME_RESULT::YELLOW;
                            player[0].tellGameState(field, player[1], winner, result);
                            player[1].tellGameState(field, player[0], winner, result);
                            GameOver = true;
                            break;

                        }

                    } catch (const PlayerError&) {
                        //Player[i] lost the connection and has lost the game. We notify the winner with an IRREGULAR_MOVE
                        auto winner = player[i].getColor() == FIELD_STATE::YELLOW ? GAME_RESULT::RED : GAME_RESULT::YELLOW;
                        player[(i + 1) % 2].tellGameState(field, player[i], winner, RESULT_REASON::IRREGULAR_MOVE);
                        return;
                    }

                    if (auto result = CheckConnectFour(field); result != GAME_RESULT::CONTINUE) {
                        player[0].tellGameState(field, player[1], result, RESULT_REASON::REGULAR);
                        player[1].tellGameState(field, player[0], result, RESULT_REASON::REGULAR);
                        GameOver = true;
                        break;
                    }
                }

                for (int i = 0; i < 7; i++){
                    for (int j = 0; j < 7; j++) {
                        switch (*field.at(std::tuple(j, i))) {
                            case FIELD_STATE::RED:
                                std::cout << 'R';
                                break;
                            case FIELD_STATE::YELLOW:
                                std::cout << 'Y';
                                break;
                            case FIELD_STATE::UNSET:
                                std::cout << '_';
                                break;
                        }
                    }
                    std::cout << '\n';
                }
                std::cout << '\n' << std::endl;
            }

        }, ConfigCounter++ % 2).detach();

    }

}




