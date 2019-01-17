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








