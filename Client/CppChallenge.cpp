//
// Created by Oliver on 17-Jan-19.
//



//Template file for the cpp challenge


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


class Connection {
private:

    SOCKET_TYPE sock;

public:


    Connection(std::uint16_t port, std::string ip) {

        //Part of exercise 2
        //Create an IPv4 TCP/IP socket using the socket() function

        //MSDN: https://docs.microsoft.com/de-de/previous-versions/technical-content/ms740506(v=vs.85)
        //Manpage: http://man7.org/linux/man-pages/man2/socket.2.html

        //Hint: In this exercise, you can read the msdn too, if you use a linux or MacOS device.
        //This is preferable, since the msdn contains way more information compared to the manpages.


        //use the connect() function to connect to the given port and IP-Address
        //You get the underlying buffer of the string object by calling ip.c_str()


    }

    ~Connection() {

        //Excercise 2
        //close the socket!

    }

    void sendInt(int i) {

        //Part of Exercise 2
        //Use the send() function
        //If you want, you can change the return value to get better error information

    }

    void sendString(std::string chars) {

        //Part of Exercise 2
        //Use the send() function
        //If you want, you can change the return value to get better error information
        //Hint: you get a pointer to the string buffer by calling chars.data() or chars.c_str()

    }


    //returns true on success
    //already implemented, do not change!
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

    explicit operator bool() const noexcept {
        return sock != static_cast<decltype(sock)>(-1);
    }

};


int main() {

    Connection conn = Connection(40596, "13.80.22.187");

    if(conn) {

        conn.sendString("{MyTeamName}");

        //TODO: Exercise 2, 3, 4, and 5

    }

    return 0;

}


