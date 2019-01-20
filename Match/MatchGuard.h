//
// Created by Oliver on 18-Jan-19.
//

#ifndef NAMEGUARD_H
#define NAMEGUARD_H


#include "ClientGame.h"

#include <mutex>
#include <unordered_set>
#include <string>
#include <optional>
#include <vector>
#include <memory>


struct Game {

    std::string TeamNames[2];

    std::uint32_t  Points[2];

};

class MatchMaking {
public:

    void checkinNewPlayer(Player&& cp);

    void pendingAgain(Player&& cp);

    void scheduleMatches();

    void executeCommand(std::string cmd);

private:

    std::mutex mtx;

    std::unordered_set<std::string> names;

    std::vector<Player> pendingPlayers;

    std::vector<std::unique_ptr<Game>> openGames;
    std::vector<std::unique_ptr<Game>> finishedGames;

};



#endif //CONNECTFOURSERVER_NAMEGUARD_H
