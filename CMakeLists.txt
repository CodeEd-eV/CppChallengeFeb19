cmake_minimum_required(VERSION 3.12)
project(ConnectFourServer)

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
    find_package(Threads REQUIRED)
endif(UNIX)

add_executable(ConnectFourServer GameServer.cpp Game/ConnectFour.cpp Game/ConnectFour.h Network/NetInit.h Network/NetInit.cpp Network/Server.cpp Network/Server.h Game/Player.cpp Game/Player.h Match/ClientGame.cpp Match/ClientGame.h)
add_executable(CompetetiveServer Game/ConnectFour.cpp Game/ConnectFour.h Network/NetInit.h Network/NetInit.cpp Network/Server.cpp Network/Server.h Game/Player.cpp Game/Player.h Match/ClientGame.h Match/ClientGame.cpp Match/MatchGuard.cpp Match/MatchGuard.h CompetetiveServer.cpp)

if (UNIX)
    target_link_libraries(ConnectFourServer Threads::Threads)
    target_link_libraries(CompetetiveServer Threads::Threads)
endif(UNIX)

add_executable(ex2 Client/Exercise2.cpp)
add_executable(ex3 Client/Exercise3.cpp)
add_executable(ex4 Client/Exercise4.cpp)
add_executable(ex5 Client/Exercise5.cpp)


