//
// Created by Oliver on 12-Jan-19.
//

#ifndef NETINIT_H
#define NETINIT_H

#include <cstdint>
#include <type_traits>


//Some differences between Windows and *nix operating systems have to be considered
#ifdef _WIN32

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")

using SOCKET_TYPE = SOCKET;

//On Windows you need to init the Network subsystem before you can use it.
class WinInitNetSubsystem {
public:

    WinInitNetSubsystem();

    ~WinInitNetSubsystem();

private:

    static WinInitNetSubsystem singleton;

};

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

inline int LastNetError();

inline void ShutdownSocket(SOCKET_TYPE sock);


enum class NET_RESULT {

    OK,
    TIMEOUT,
    FAILED,

};

class Connection {
public:

    explicit Connection(SOCKET_TYPE s) noexcept;

    Connection(Connection&&) noexcept;
    Connection& operator=(Connection&&) noexcept;

    template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>>
    NET_RESULT receive(T&);

    template<typename T>
    NET_RESULT receive(T* data, std::int32_t bytes);

    template<typename T, typename = std::enable_if_t<std::is_pod_v<T>>>
    NET_RESULT send(const T& s);

    template<typename T>
    NET_RESULT send(const T* data, std::int32_t bytes);

    explicit operator bool() const noexcept;

    void close();

    ~Connection();

private:

    SOCKET_TYPE sock;

};

template<typename T, typename>
NET_RESULT Connection::receive(T& t) {

    if (auto r = recv(sock, reinterpret_cast<char*>(&t), sizeof(std::remove_reference_t<T>), 0); r > 0) {

        return NET_RESULT::OK;


    } else if(r == 0) {

        return NET_RESULT::FAILED;

    } else {

        return NET_RESULT::TIMEOUT;

    }

}

template<typename T>
NET_RESULT Connection::receive(T *data, std::int32_t bytes) {

    if(auto r = recv(sock, reinterpret_cast<char*>(data), bytes, 0); r > 0) {

        return NET_RESULT::OK;


    } else if(r == 0) {

        return NET_RESULT::FAILED;

    } else {

        return NET_RESULT::TIMEOUT;

    }

}

template<typename T, typename>
NET_RESULT Connection::send(const T& t) {

    return send(&t, sizeof(T));

}

template<typename T>
NET_RESULT Connection::send(const T* t, std::int32_t bytes) {

    if(auto s = ::send(sock, reinterpret_cast<const char*>(t), bytes, 0); s > 0) {

        return NET_RESULT::OK;

    } else {

        return NET_RESULT::FAILED;

    }

}





#endif //NETINIT_H