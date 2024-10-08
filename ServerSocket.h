#pragma once

#include "Socket.h"
#include "TLSSocket.h"
#include "SocketHelper.h"

#include <string>
#include <memory>

class ServerSocket
{
	    int m_socket = -1;
        bool m_isSecure = false;

    public:

        ServerSocket():m_socket(-1){}
		ServerSocket(const std::string& address, const std::string &family, int port, bool isBlock = false, bool isSecure = false);
        ~ServerSocket();

        std::unique_ptr<Socket> Accept(const Acceptor& acceptor);
        int Get() const;
        bool IsSecure() const;

        ServerSocket& operator= (ServerSocket&& t);
        ServerSocket (ServerSocket&& t);

        ServerSocket(const ServerSocket&) = delete;
        ServerSocket& operator= (const ServerSocket&) = delete;

};
