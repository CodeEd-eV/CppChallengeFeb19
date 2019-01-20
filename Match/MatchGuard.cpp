//
// Created by Oliver on 18-Jan-19.
//

#include "MatchGuard.h"

#include <fstream>


std::vector<std::string> ReadNamesFromFile(std::string filepath) {

    std::ifstream file(filepath);

    std::vector<std::string> names;
    if(file) {

        while(!file.eof()) {

            std::string name;
            std::getline(file, name);

            names.push_back(std::move(name));

        }

    }

    return names;

}

std::vector<Game> GenerateGamesFromNameList(const std::vector<std::string>& names) {

    std::vector<Game> games;

    for(std::size_t i = 0; i < names.size(); i++) {
        for (std::size_t j = 0; j < names.size(); j++) {
            if(i == j) {
                continue;
            }

            games.push_back({false, {names[i], names[j]}, {0, 0}});

        }
    }

    return games;

}



MatchMaking::MatchMaking(const std::vector<std::string> &nms) {

    games = GenerateGamesFromNameList(nms);

    for(auto& e : nms) {

        names.insert(e);

    }

}

void MatchMaking::checkinNewPlayer(Player &&cp) {

    std::lock_guard lck(mtx);

    if (auto it = names.find(cp.getName()); it != std::end(names)) {

        names.erase(it);
        pendingPlayers.push_back(std::move(cp));

    }

}

void MatchMaking::checkoutPlayer(std::string name) {

    std::lock_guard lck(mtx);

    names.insert(std::move(name));

}

void MatchMaking::setAsPending(Player &&cp) {

    std::lock_guard lck(mtx);

    pendingPlayers.push_back(std::move(cp));

}

void lookForMatches() {




}



