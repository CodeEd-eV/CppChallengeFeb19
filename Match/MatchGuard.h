//
// Created by Oliver on 18-Jan-19.
//

#ifndef NAMEGUARD_H
#define NAMEGUARD_H


#include "ClientGame.h"

#include <mutex>
#include <unordered_map>
#include <string>
#include <optional>
#include <vector>


struct Game {

    enum STATE {
        PENDING,
        RUNNING,
        FINISHED
    };

    STATE GameState = PENDING;

    std::string TeamNames[2];

    std::uint32_t  Points[2];

};

std::vector<std::string> ReadNamesFromFile(std::string filepath);

enum class PLAYER_STATE {

    PENDING,
    PLAYING,
    CONNECTION_LOST,

};

struct GameResult {

    inline GameResult(Game& g) : game(g) {};

    Game& game;
    struct {

        std::string Name;
        std::uint32_t Points;
        PLAYER_STATE State;

    }PlayerInfo[2];



};

class MatchMaking {
public:

    explicit MatchMaking(const std::vector<std::string>& names);

    void checkinNewPlayer(Player&& cp);

    void matchAndRunGames();

    void onGameReturn(GameResult gr);

    void executeCommand(const std::string& cmd);

private:

    std::mutex mtx;

    struct PlayerState {

        PLAYER_STATE State;
        Player player;

    };
    std::unordered_map<std::string, std::optional<PlayerState>> players;

    std::vector<Game> games;

};



#endif //CONNECTFOURSERVER_NAMEGUARD_H
