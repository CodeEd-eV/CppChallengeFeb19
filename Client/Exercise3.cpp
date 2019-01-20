//
// Created by Oliver on 18-Jan-19.
//


//Very naive and simple solution to exercise3!

#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using SOCKET_TYPE = SOCKET;

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

#endif //_WIN32

#include <cstring>
#include <string>
#include <iostream>

//Start reading here, ignore above code.

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
    public:
        std::string Name;
        FIELD_STATE Color; //This value is either YELLOW or RED, never UNSET!
    }Player1, Player2;

    //Indicates if the game is over and who has won it, otherwise its value is GAME_RESULT::CONTINUE
    GAME_RESULT Result;

    /*
     * The field is stored in column major order!
     * Field[0][0], Field[1][0], Field[2][0], .... , Field[6][0] <-- Top Row
     * Field[0][1], Field[1][1],            , .... , Field[6][1]
     * Field[0][2], Field[1][2], .....
     * .....
     * .....
     * Field[0][6], Field[1][6], .....             , Field[6][6] <-- Bottom Row
     */
    FIELD_STATE Field[7][7];

    //Ignore, when Result is CONTINUE
    RESULT_REASON Reason;



};


void PrintGameInfo(GameInfo gi) {

    std::cout << "Player1: " << gi.Player1.Name << ((gi.Player1.Color == FIELD_STATE::RED) ? " Red\n" : " Yellow\n");
    std::cout << "Player2: " << gi.Player2.Name << ((gi.Player2.Color == FIELD_STATE::RED) ? " Red\n" : " Yellow\n");

    switch(gi.Result) {
        case GAME_RESULT::CONTINUE:
            std::cout << "Continue game\n";
            break;
        case GAME_RESULT::YELLOW:
            switch(gi.Reason) {
                case RESULT_REASON::IRREGULAR_MOVE:
                    std::cout << "Yellow won, reds move was invalid\n";
                    break;

                case RESULT_REASON::REGULAR:
                    std::cout << "Yellow won\n";
                    break;

                case RESULT_REASON::TIMEOUT:
                    std::cout << "Yellow won, red timed out\n";
                    break;
            }
            break;

        case GAME_RESULT::RED:
            switch(gi.Reason) {
                case RESULT_REASON::IRREGULAR_MOVE:
                    std::cout << "Red won, yellows move was invalid\n";
                    break;

                case RESULT_REASON::REGULAR:
                    std::cout << "Red won\n";
                    break;

                case RESULT_REASON::TIMEOUT:
                    std::cout << "Red won, yellow timed out\n";
                    break;
            }
            break;

        case GAME_RESULT::DRAW:
            std::cout << "Draw\n";
            break;
    }

    for (int i = 0; i < 7; i++) {
        for (int j = 0; j < 7; j++) {
            switch (gi.Field[j][i]) {
                case FIELD_STATE::UNSET:
                    std::cout << '_';
                    break;

                case FIELD_STATE::YELLOW:
                    std::cout << 'Y';
                    break;

                case FIELD_STATE::RED:
                    std::cout << 'R';
                    break;

            }
        }
        std::cout << '\n';
    }

    std::cout << std::endl;

}

class Connection {
public:

    Connection(std::uint16_t port, std::string ip) {

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(sock != -1) {

            in_addr inAddr;
            inet_pton(AF_INET, ip.c_str(), &inAddr);

            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr = inAddr; //inet_addr works too, but is deprected!
            serverAddr.sin_port = htons(port);

            if(connect(sock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == -1) {

#ifdef _WIN32
                closesocket(sock);
#else
                close(sock);
#endif
                sock = -1;

            }

        }


    }

    void sendInt(int i) {

        send(sock, reinterpret_cast<char*>(&i), sizeof(i), 0);

    }

    void sendString(std::string chars) {

        send(sock, chars.c_str(), chars.size(), 0);

    }

    ~Connection() {

        if(sock != -1) {

#ifdef _WIN32
            closesocket(sock);
#else
            close(sock);
#endif

            sock = -1;
        }

    }

    //returns true on success
    bool receive(GameInfo* gi) {

        if (gi == nullptr) {
            return false;
        }

        struct alignas(1) LLGameInfo {
            struct alignas(1) PlayerInfo {
                char Name[15];
                FIELD_STATE Color;
            }Player[2];
            GAME_RESULT Result;
            FIELD_STATE Field[7][7];
            RESULT_REASON Reason;

        }llgi;

        if(recv(sock, reinterpret_cast<char*>(&llgi), sizeof(GameInfo), 0) <= 0) {

            return false;

        }

        gi->Player1.Color = llgi.Player[0].Color;
        gi->Player1.Name = llgi.Player[0].Name;
        gi->Player2.Color = llgi.Player[1].Color;
        gi->Player2.Name = llgi.Player[1].Name;
        gi->Result = llgi.Result;
        std::memcpy(gi->Field, llgi.Field, sizeof(LLGameInfo::Field));
        gi->Reason = llgi.Reason;

        return true;

    }


    operator bool() {

        return sock != static_cast<decltype(sock)>(-1);

    }

private:

    SOCKET_TYPE sock;

};


int main() {

    Connection conn = Connection(40596, "127.0.0.1");
    if(conn) {

        conn.sendString("{MyTeamName}");

        GameInfo gameInfo;
        while(true) {

            conn.receive(&gameInfo);

            if(gameInfo.Result != GAME_RESULT::CONTINUE) {
                break;
            }

            PrintGameInfo(gameInfo);

            int x;
            std::cin >> x;
            conn.sendInt(x);

        }

        std::cout << "Game Over!\n";
        PrintGameInfo(gameInfo);

    }

    return 0;

}








