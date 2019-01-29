//
// Created by Oliver on 27-Jan-19.
//



#include "Network/Server.h"
#include "Match/MatchGuard.h"
#include "Match/ClientGame.h"

#include <thread>
#include <iostream>



int main(int argc, char** argv) {

    /*
    if(argc < 2) {
        std::cout << "Specify Team list!" << std::endl;
        return -1;
    }
    */

    std::vector<std::string> names = {"Team1", "Team2"};
    //auto names = ReadNamesFromFile(argv[1]);

    MatchMaking mk(names);

    auto server = Server(40596);
    std::thread([&mk, &server] {

        server.init(10);

        for(auto refConnection : server) {

            std::thread([&mk, connection = std::move(refConnection.get())] () mutable {

                char nameBuffer[15] = {0};
                if(connection.receive(nameBuffer, sizeof(nameBuffer) - 1) != NET_RESULT::OK) {
                    return;
                }

                ClientPlayer cp(nameBuffer, std::move(connection));
                mk.checkinNewPlayer(Player(std::move(cp), FIELD_STATE::RED));


            }).detach();

        }

    }).detach();

    for(;;) {

        std::string cmd;
        std::getline(std::cin, cmd);

        if(cmd == "server shutdown") {
            server.shutdown();
            break;
        } else {

            mk.executeCommand(cmd);

        }

    }


}





