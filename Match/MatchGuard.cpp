//
// Created by Oliver on 18-Jan-19.
//

#include "MatchGuard.h"
#include "ClientGame.h"


#include <thread>
#include <fstream>
#include <iostream>
#include <array>


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

            games.push_back({Game::PENDING, {names[i], names[j]}, {0, 0}});

        }
    }

    return games;

}

MatchMaking::MatchMaking(const std::vector<std::string> &nms) {

    games = GenerateGamesFromNameList(nms);

    for(auto& e : nms) {

        players.insert({e, std::nullopt });

    }

}

void MatchMaking::checkinNewPlayer(Player &&cp) {

    std::lock_guard lck(mtx);

    if (auto it = players.find(cp.getName()); it != std::end(players)) {

        if(!it->second.has_value()) {

            it->second = {PLAYER_STATE::PENDING, std::move(cp)};

        }

    }

    matchAndRunGames();

}

void MatchMaking::onGameReturn(GameResult gr) {

    std::lock_guard lck(mtx);

    gr.game.GameState = Game::FINISHED;
    for(int i : {0, 1}) {
        gr.game.Points[i] = gr.PlayerInfo[i].Points;

        if(gr.PlayerInfo[i].State == PLAYER_STATE::CONNECTION_LOST) {
            if(auto it = players.find(gr.PlayerInfo[i].Name); it != std::end(players)) {
                it->second = std::nullopt;
            }
        }
        else {
            if(auto it = players.find(gr.PlayerInfo[i].Name); it != std::end(players)) {
                it->second.value().State = PLAYER_STATE::PENDING;
            }
        }
    }

    matchAndRunGames();

}

void MatchMaking::matchAndRunGames() {

    for(auto& g : games) {

        if(g.GameState == Game::PENDING) {

            auto firstPlayer = players.find(g.TeamNames[0]);
            auto secondPlayer = players.find(g.TeamNames[1]);

            if (firstPlayer != std::end(players) && secondPlayer != std::end(players)) {

                if (firstPlayer->second.has_value() && secondPlayer->second.has_value()) {

                    if ((*firstPlayer->second).State == PLAYER_STATE::PENDING &&
                        (*secondPlayer->second).State == PLAYER_STATE::PENDING) {

                        (*firstPlayer->second).State = PLAYER_STATE::PLAYING;
                        (*secondPlayer->second).State = PLAYER_STATE::PLAYING;
                        g.GameState = Game::RUNNING;

                        std::thread([this, player = std::array<Player*, 2>{&(*firstPlayer->second).player, &(*secondPlayer->second).player}] (Game& g) {

                            GameResult gameResult(g);
                            for(int i : {0, 1}) {
                                gameResult.PlayerInfo[i].Name = player[i]->getName();
                                gameResult.PlayerInfo[i].State = PLAYER_STATE::PLAYING;
                                gameResult.PlayerInfo[i].Points = 0;
                            }

                            player[0]->setColor(FIELD_STATE::RED);
                            player[1]->setColor(FIELD_STATE::YELLOW);

                            ConnectFour field;
                            bool GameOver = false;
                            while(!GameOver) {

                                for (int i : {0, 1}) {

                                    try {

                                        if (auto result = Move(field, *player[i], *player[(i + 1) % 2]); result !=
                                                                                                       RESULT_REASON::REGULAR) {
                                            auto winner = player[i]->getColor() == FIELD_STATE::YELLOW ?
                                                          GAME_RESULT::RED : GAME_RESULT::YELLOW;
                                            player[0]->tellGameState(field, *player[1], winner, result);
                                            player[1]->tellGameState(field, *player[0], winner, result);
                                            std::cout << player[i]->getName() << " did an irregular move" << std::endl;
                                            gameResult.PlayerInfo[i].Points = 0;
                                            gameResult.PlayerInfo[(i + 1) % 2].Points = 3;
                                            GameOver = true;
                                            break;

                                        }

                                    } catch(const PlayerError&) {

                                        auto winner = (player[i]->getColor() == FIELD_STATE::YELLOW ? GAME_RESULT::RED : GAME_RESULT::YELLOW);
                                        player[(i + 1) % 2]->tellGameState(field, *player[i], winner, RESULT_REASON::TIMEOUT);
                                        gameResult.PlayerInfo[i].Points = 0;
                                        gameResult.PlayerInfo[(i + 1) % 2].Points = 3;
                                        GameOver = true;
                                        break;

                                    }

                                    if (auto result = CheckConnectFour(field); result != GAME_RESULT::CONTINUE) {
                                        player[0]->tellGameState(field, *player[1], result, RESULT_REASON::REGULAR);
                                        player[1]->tellGameState(field, *player[0], result, RESULT_REASON::REGULAR);
                                        if(result == GAME_RESULT::DRAW) {
                                            gameResult.PlayerInfo[0].Points = 1;
                                            gameResult.PlayerInfo[1].Points = 1;
                                        }
                                        else {
                                            gameResult.PlayerInfo[i].Points = 3;
                                            gameResult.PlayerInfo[(i + 1) % 2].Points = 0;
                                        }

                                        GameOver = true;
                                        break;
                                    }

                                }

                            }

                            this->onGameReturn(gameResult);

                        }, g).detach();

                    }
                }
            }
        }
    }
}


