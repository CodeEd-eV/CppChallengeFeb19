//
// Created by Oliver on 18-Jan-19.
//

#include "ClientGame.h"



#include <iostream>

ClientPlayer::ClientPlayer(std::string nm, Connection&& con) : name(std::move(nm)), connection(std::move(con)) {

}


std::uint32_t ClientPlayer::chooseMove(ConnectFour& cf) {

    std::uint32_t x;
    switch(connection.receive(x)) {
        case NET_RESULT::OK:
            return x;

        case NET_RESULT::TIMEOUT: //a timed out connection is in a unstable state (according to msdn),
        std::cout << "Timeout\n";
        case NET_RESULT::FAILED:  //so better treat it as failed. (X) but ok.
            throw PlayerError();

    }
    return x;


}

void ClientPlayer::tellGameState(const LLGameInfo& gameInfo) {

    connection.send(gameInfo);
    
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



void SingleGame(std::array<Player, 2>& player) {

    ConnectFour field;
    bool GameOver = false;
    while(!GameOver) {

        for (int i = 0; i < 2; i++) {

            try {

                if (auto result = Move(field, player[i], player[(i + 1) % 2]); result !=
                                                                               RESULT_REASON::REGULAR) {
                    auto winner = player[i].getColor() == FIELD_STATE::YELLOW ?
                                                            GAME_RESULT::RED : GAME_RESULT::YELLOW;
                    player[0].tellGameState(field, player[1], winner, result);
                    player[1].tellGameState(field, player[0], winner, result);
                    std::cout << player[i].getName() << " did an irregular move" << std::endl;
                    GameOver = true;
                    break;

                }

            } catch (const PlayerError &) {
                //Player[i] lost the connection or timed out and has lost the game.
                auto winner = (player[i].getColor() == FIELD_STATE::YELLOW ? GAME_RESULT::RED : GAME_RESULT::YELLOW);
                player[(i + 1) % 2].tellGameState(field, player[i], winner, RESULT_REASON::TIMEOUT);
                throw;
            }

            if (auto result = CheckConnectFour(field); result != GAME_RESULT::CONTINUE) {
                player[0].tellGameState(field, player[1], result, RESULT_REASON::REGULAR);
                player[1].tellGameState(field, player[0], result, RESULT_REASON::REGULAR);
                GameOver = true;
                break;
            }

        }

    }


}



