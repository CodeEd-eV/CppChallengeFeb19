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

    bool Finished;

    std::string TeamNames[2];

    std::uint32_t  Points[2];

};

std::vector<std::string> ReadNamesFromFile(std::string filepath);

class MatchMaking {
public:

    explicit MatchMaking(const std::vector<std::string>& names);

    void checkinNewPlayer(Player&& cp);

    void checkoutPlayer(std::string name);

    void setAsPending(Player&& cp1);

    void lookForMatches();

    void executeCommand(std::string cmd);

private:

    void scheduleMatch(Player&& cp1, Player&& cp2);

    void setMatchResult(const std::string& p1, const std::string& p2, int pts1, int pts2);

    std::mutex mtx;

    std::unordered_set<std::string> names;

    std::list<Player> pendingPlayers;

    std::vector<Game> games;

};



#endif //CONNECTFOURSERVER_NAMEGUARD_H
