//
// Created by Oliver on 18-Jan-19.
//

#include "MatchGuard.h"
#include "ClientGame.h"


#include <thread>
#include <fstream>
#include <iostream>
#include <iomanip>
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
            std::cout << gr.PlayerInfo[i].Name << " lost connection!\n";
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

                        //std::thread forwards argument by a decay_copy so you have to use a reference_wrapper
                        std::thread([this, player = std::array<Player*, 2>{&(*firstPlayer->second).player,&(*secondPlayer->second).player}] (std::reference_wrapper<Game> g) {

                            GameResult gameResult(g.get());
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

                        }, std::ref(g)).detach();

                    }
                }
            }
        }
    }
}

void MatchMaking::executeCommand(const std::string &cmd) {

    std::lock_guard lck(mtx);

    if(cmd == "Print Table") {

        struct TableEntry {

            std::string Name;
            int Points;
            int TotalGamesPlayed;
            int GameWon;
            int Draws;
            int GamesLost;


        };

        std::vector<TableEntry> table;

        for(const auto& p : players) {
            TableEntry tb = {p.first, 0, 0, 0, 0, 0};

            for(const auto& g: games) {

                if(g.GameState == Game::FINISHED) {
                    for (int i : {0, 1}) {
                        if (p.first == g.TeamNames[i]) {

                            tb.Points += g.Points[i];
                            tb.TotalGamesPlayed++;
                            tb.Draws += g.Points[i] == 1 ? 1 : 0;
                            tb.GameWon += g.Points[i] == 3 ? 1 : 0;
                            tb.GamesLost += g.Points[i] == 0 ? 1 : 0;
                        }

                    }
                }

            }
            table.push_back(tb);

        }



        std::sort(std::begin(table), std::end(table), [](const auto& e1, const auto& e2) {

            return e1.Points < e2.Points;

        });

        std::cout << std::setw(20) << "Name" << std::setw(10) <<"Points"
                  << std::setw(10) << "Games won" << std::setw(10) << "draws"
                  << std::setw(10) << "Games lost" << std::setw(15) << "games played" << std::endl;
        for(const auto& t : table) {
            std::cout << std::setw(20) << t.Name << std::setw(10) << t.Points
                        << std::setw(10) << t.GameWon << std::setw(10) << t.Draws
                        << std::setw(10) << t.GamesLost << std::setw(15) << t.TotalGamesPlayed << std::endl;

        }

    }
    else if (cmd == "Print Games") {

        for(const auto& e: games) {
            std::cout << std::setw(15) << e.TeamNames[0] << "[" << e.Points[0] << "]"
                        << " vs. " << std::setw(15) << e.TeamNames[1] << "[" << e.Points[1] << "] ";

            switch(e.GameState) {
                case Game::PENDING:
                    std::cout << "Pending ";
                    break;
                case Game::RUNNING:
                    std::cout << "Running ";
                    break;
                case Game::FINISHED:
                    std::cout << "Finished ";
                    break;
            }
            std::cout << "\n";
        }

    }
    else if (cmd == "Print Players") {

        for(const auto& e : players) {

            std::cout << e.first << (e.second.has_value() ? "Connected" : "Disconnected");


        }


    }


}


