//
// Created by Oliver on 12-Jan-19.
//


#include "NetInit.h"

#ifdef _WIN32

WinInitNetSubsystem::WinInitNetSubsystem() {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

}

WinInitNetSubsystem::~WinInitNetSubsystem() {

    WSACleanup();

}

WinInitNetSubsystem WinInitNetSubsystem::singleton = WinInitNetSubsystem();

#endif

void ShutdownSocket(SOCKET_TYPE sock) {

#ifdef _WIN32

    closesocket(sock);

#else

    close(sock);

#endif

}






Connection::Connection(SOCKET_TYPE s) noexcept : sock(s) {

}

Connection::Connection(Connection&& sc) noexcept : sock(sc.sock){

    sc.sock = NET_SOCK_ERR;

}

Connection& Connection::operator=(Connection&& rhs) noexcept {

    sock = rhs.sock;
    rhs.sock = NET_SOCK_ERR;

    return *this;

}

Connection::operator bool() const noexcept {

    return sock != NET_SOCK_ERR;

}

void Connection::close() {

    if (*this) {

        ShutdownSocket(sock);
        sock = NET_SOCK_ERR;
    }

}

Connection::~Connection() {

   close();

}


