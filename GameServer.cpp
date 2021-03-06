
#include "Network/Server.h"
#include "Match/ClientGame.h"

#include <iostream>
#include <thread>


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

            auto player = CreateRandomAiAndClientPlayer(ClientPlayer(nameBuffer, std::move(connection)),
                                                        config % 2);

            try {

                for (std::uint32_t ngames = 0; ngames < 2 + (config % 5); ngames++) { //Play a few games

                    SingleGame(player);

                    if (ngames % 2) { //Swap players every other game
                        std::swap(player[0], player[1]);
                        player[0].setColor(FIELD_STATE::RED);
                        player[1].setColor(FIELD_STATE::YELLOW);
                    }

                }

            }
            catch(const PlayerError&) {

            }

        }, ConfigCounter++).detach();

    }

}


