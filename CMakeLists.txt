cmake_minimum_required(VERSION 3.12)
project(ConnectFourServer)

set(CMAKE_CXX_STANDARD 17)

add_executable(ConnectFourServer GameServer.cpp Game/ConnectFour.cpp Game/ConnectFour.h Network/NetInit.h Network/NetInit.cpp Network/Server.cpp Network/Server.h Game/Player.cpp Game/Player.h Match/MatchGuard.cpp Match/MatchGuard.h Match/ClientGame.cpp Match/ClientGame.h)

#add_executable(CompetetiveServer CompetitiveServer.cpp Game/ConnectFourField.h Game/ConnectFourField.cpp Game/GameRule.cpp Game/GameRule.h)


add_executable(ex2 Client/Exercise2.cpp)
add_executable(ex3 Client/Exercise3.cpp)
add_executable(ex4 Client/Exercise4.cpp)


#add_executable(ManualClient)

#add_executable(AutoClient)

