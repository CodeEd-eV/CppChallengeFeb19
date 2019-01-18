//
// Created by Oliver on 17-Jan-19.
//

#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using SOCKET_TYPE = SOCKET;

//On Windows you need to init the Network subsystem before you can use it.
//We use a singleton to ensure the initialization is done at the very first step of your program.
//You can ignore this code, if you are not familiar with C++
class WinInitNetSubsystem {
public:

    WinInitNetSubsystem() {

        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);

    }

    ~WinInitNetSubsystem() {

        WSACleanup();

    }

private:

    static WinInitNetSubsystem singleton;

};

WinInitNetSubsystem WinInitNetSubsystem::singleton = WinInitNetSubsystem();

#else //Linux, MacOS, ....

#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using SOCKET_TYPE = int;

constexpr int SOCKET_ERROR = -1;

#endif //_WIN32

constexpr auto NET_SOCK_ERR = static_cast<SOCKET_TYPE>(SOCKET_ERROR);


enum class FIELD_STATE : std::uint8_t {

    UNSET = 0,
    YELLOW = 1,
    RED = 2,

};

enum class GAME_RESULT : std::uint8_t {

    YELLOW,
    RED,
    DRAW,
    CONTINUE

};

enum class RESULT_REASON : std::uint8_t {

    REGULAR, //Regular game over, either it is a draw or the winner team connected four
    TIMEOUT, //The looser team did not send a move command within 5 seconds
    IRREGULAR_MOVE, //The looser team send an invalid command

};

class GameInfo {
public:

    class Player {
        std::string Name;
        FIELD_STATE Color; //This value is either YELLOW or RED, never UNSET!
    }Player1, Player2;

    //Indicates if the game is over and who has won it, otherwise its value is GAME_RESULT::CONTINUE
    GAME_RESULT Result;

    /*
     * The field is stored in column major order!
     * [0][0], [1][0], [2][0], .... , [6][0] <-- Top Row
     * [0][1], [1][1], .....        , [6][1]
     * [0][2], [1][2], .....
     * .....
     * .....
     * [0][6], [1][6], .....         , [6][6] <-- Bottom Row
     */
    FIELD_STATE Field[7][7];

    //Ignore, when Result is CONTINUE
    RESULT_REASON Reason;



};






