//
// Created by Oliver on 18-Jan-19.
//

#ifndef CONNECTFOURSERVER_CLIENTPLAYER_H
#define CONNECTFOURSERVER_CLIENTPLAYER_H


#include "../Network/NetInit.h"
#include "../Game/ConnectFour.h"
#include "../Game/Player.h"

#include <string>

class ClientPlayer {
public:

    ClientPlayer(std::string nm, Connection&& con);

    std::uint32_t chooseMove(ConnectFour& cf);

    void tellGameState(const LLGameInfo& gameInfo);

    const std::string& getName() const;

private:

    std::string name;
    Connection connection;

};


std::array<Player, 2> CreateRandomAiAndClientPlayer(ClientPlayer&& cp, std::uint32_t config);


void SingleGame(std::array<Player, 2>& players);

void MultipleGames(std::array<Player, 2> players, std::uint32_t nGames);



#endif //CONNECTFOURSERVER_CLIENTPLAYER_H
