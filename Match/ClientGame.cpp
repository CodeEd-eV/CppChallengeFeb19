//
// Created by Oliver on 18-Jan-19.
//

#include "ClientGame.h"





ClientPlayer::ClientPlayer(std::string nm, Connection&& con) : name(std::move(nm)), connection(std::move(con)) {

}


std::uint32_t ClientPlayer::chooseMove(ConnectFour& cf) {

    char mv[2] = {0};
    connection.receive(mv);
    return (std::uint32_t) std::atoi(mv);


}

void ClientPlayer::tellGameState(const LLGameInfo& gameInfo) {

}

const std::string& ClientPlayer::getName() const {

    return name;

}



std::array<Player, 2> CreateRandomAiAndClientPlayer(ClientPlayer&& cp, std::uint32_t config) {

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


#include <iostream>

void SingleGame(std::array<Player, 2> player) {

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

            } catch (const PlayerError &) {
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


}



