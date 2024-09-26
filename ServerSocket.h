#pragma once
#include <string>

class ServerSocket
{
	    int m_socket;

    public:
        ServerSocket():m_socket(-1){}
		ServerSocket(const std::string& address, const std::string &family, int port);
        ~ServerSocket();

        int Get() const;

        ServerSocket(const ServerSocket&) = delete;
        ServerSocket& operator= (const ServerSocket&) = delete;
        ServerSocket& operator= (ServerSocket&& t);
    };
