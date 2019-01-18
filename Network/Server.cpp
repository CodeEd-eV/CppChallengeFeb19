//
// Created by Oliver on 15-Jan-19.
//

#include "Server.h"



Server::Server(std::uint16_t port) noexcept : listenSocket(NET_SOCK_ERR), port(port), recvTimeout(5) {

}

Server::~Server() {

    if (listenSocket != SOCKET_ERROR) {
        ShutdownSocket(listenSocket);
        listenSocket = SOCKET_ERROR;
    }

}

void Server::init(std::uint32_t tm) {

    recvTimeout = tm;

    listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocket == SOCKET_ERROR) {

        return;

    }

    sockaddr_in sockrecv = {0};
    sockrecv.sin_family = AF_INET;
    sockrecv.sin_addr.s_addr = INADDR_ANY;
    sockrecv.sin_port = htons(port);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&sockrecv), sizeof(sockrecv)) == SOCKET_ERROR) {

        return;

    }


    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) {

        return;

    }


}

void Server::shutdown() {

#ifdef _WIN32

    ::shutdown(listenSocket, SD_BOTH);

#else

    ::shutdown(listenSocket, SHUT_RDWR);

#endif

}

Server::socket_iterator Server::begin() {

    auto it = socket_iterator(*this);

    it.resetConnection();

    return it;

}

Server::socket_iterator Server::end() {

    return socket_iterator(*this);

}

SOCKET_TYPE Server::accept() {

    auto asock =  ::accept(listenSocket, nullptr, nullptr);
    if(asock == NET_SOCK_ERR) {
        return asock;
    }


#ifdef _WIN32

    DWORD timeout = 1000 * recvTimeout;

#else

    timeval timeout = {0};
    timeout.tv_sec = recvTimeout;

#endif

    //Sets a receive timeout to the socket
    setsockopt(asock, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));


    return asock;

}



Server::socket_iterator::socket_iterator(Server &serv) noexcept : connection(NET_SOCK_ERR), acceptor(serv) {

}

void Server::socket_iterator::operator++() {

    resetConnection();

}

bool Server::socket_iterator::operator==(const Server::socket_iterator &rhs) const {

    //Otherwise after inc the begin iterator will equals the end iterator
    return false;

}

bool Server::socket_iterator::operator!=(const Server::socket_iterator &rhs) const {

    return !(*this == rhs);

}

std::reference_wrapper<Connection> Server::socket_iterator::operator*() {

    return std::ref(connection);

}

void Server::socket_iterator::resetConnection() {

    connection = Connection(acceptor.accept());

}



