#pragma once

#include <string>
#include <vector>
#include <stop_token>

#include "ServerSocket.h"

class Server 
{
	    int m_epoll = -1;
        std::vector<ServerSocket> m_serverSockets;
        TLSContext m_tlsContext;

	public:
        
		Server ();
		~Server ();

		std::unique_ptr<Socket> WaitClients(std::stop_token st);
        void CreateEpoll();
        void AddSocket(ServerSocket &s);

		Server(const Server&) = delete;
		Server& operator= (const Server&) = delete;

    private:
        
        void AddSocket (int socket);
        void DeleteSocket (int socket);
};
