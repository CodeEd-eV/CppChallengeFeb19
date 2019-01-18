//
// Created by Oliver on 15-Jan-19.
//

#ifndef SERVER_H
#define SERVER_H

#include "NetInit.h"

#include <iterator>
#include <functional> //reference_wrapper


class Server {
public:

    //Use only with ranged based for!
    class socket_iterator {
        friend class Server;
    public:

        using value_type = Connection;
        using pointer = const Connection*;
        using reference = std::reference_wrapper<const Connection>;
        using iterator_category = std::forward_iterator_tag;

        socket_iterator(socket_iterator&& mv) noexcept = default;
        socket_iterator& operator=(socket_iterator&& rhs) noexcept = default;

        void operator++();

        bool operator==(const socket_iterator& rhs) const;
        bool operator!=(const socket_iterator& rhs) const;

        std::reference_wrapper<Connection> operator*();

        ~socket_iterator() = default;

    private:

        explicit socket_iterator(Server& serv) noexcept;

        void resetConnection();

        Server& acceptor;
        Connection connection;

    };

    explicit Server(std::uint16_t port) noexcept;

    void init(std::uint32_t timeout = 5);

    void shutdown();

    socket_iterator begin();

    socket_iterator end();

    ~Server();

private:

    SOCKET_TYPE accept();

    std::uint32_t recvTimeout;
    std::uint16_t  port;
    SOCKET_TYPE listenSocket;


};




#endif
