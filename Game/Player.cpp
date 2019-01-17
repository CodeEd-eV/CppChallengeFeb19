//
// Created by Oliver on 17-Jan-19.
//

#include "Player.h"

LLGameInfo MakeGameInfo(const ConnectFour& cf, const std::string& plname1,
                        FIELD_STATE col1, const std::string& plname2, FIELD_STATE col2,
                        GAME_RESULT result, RESULT_REASON rr, bool playAgain) {

    LLGameInfo gi = {0};

    std::memcpy(gi.Player[0].Name, plname1.data(), sizeof(gi.Player[0].Name) - 1);
    gi.Player[0].Color = col1;

    std::memcpy(gi.Player[1].Name, plname2.data(), sizeof(gi.Player[1].Name) - 1);
    gi.Player[1].Color = col2;

    std::memcpy(gi.Field, &cf, sizeof(ConnectFour));

    gi.Result = result;
    gi.Reason = rr;
    gi.PlayAgain = playAgain;

    return gi;

}


const std::string& Player::getName() const {

    return playerPtr->getName();

}

std::uint32_t Player::chooseMove(ConnectFour &cf) {

    return playerPtr->chooseMove(cf);

}

FIELD_STATE  Player::getColor() const {

    return playerPtr->getColor();

}

void Player::tellGameState(ConnectFour &cf, const Player &other, GAME_RESULT result, RESULT_REASON rr, bool playAgain) {

    playerPtr->tellGameState(cf, other, result, rr, playAgain);


}

void Player::setColor(FIELD_STATE col) {

    playerPtr->setColor(col);

}

#include <random>
#include <chrono>

Player MakeRandomAIPlayer(FIELD_STATE color, std::uint32_t pnr) {

    class RandomPlayer {
    public:

        explicit RandomPlayer(std::string nm) :
                name(std::move(nm)), eng(std::chrono::system_clock::now().time_since_epoch().count()) {

        }

        const std::string& getName() const {

            return name;

        }

        std::uint32_t chooseMove(ConnectFour& cf) {

            while(true) {

                auto index = dis(eng);
                if(cf[index].isFull()) {
                    continue;
                }
                return index;

            }

        }

        void tellGameState(const LLGameInfo& gameInfo) {

            //Do nothing

        }

    private:

        std::default_random_engine eng;
        std::uniform_int_distribution<std::uint32_t> dis{0, 3};//{0, ConnectFour::WIDTH<std::uint32_t> - 1};

        std::string name;

    };

    return Player(RandomPlayer("RandomAI"), color, pnr);

}





