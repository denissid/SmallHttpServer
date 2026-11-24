#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <stop_token>

#include "Socket.h"
#include "ServerSocket.h"
#include "IOContext.h"

class Server 
{

	    int m_epoll = -1;
 //       std::unordered_map<int, std::unique_ptr<Socket> > m_sockets;
        std::unordered_map<int, std::unique_ptr<IOContext> > m_contexts;
        std::unordered_map<int, std::unique_ptr<ServerSocket> > m_serverSockets;
        TLSContext m_tlsContext;

	public:
        
           
		Server ();
		~Server ();

		std::unique_ptr<IOContext> WaitClients(std::stop_token st);
        void CreateEpoll();

        void AddSocket(std::unique_ptr<ServerSocket> s);
        void AddSocket(SocketType st, bool isUpdate, std::unique_ptr<IOContext> c);

		Server(const Server&) = delete;
		Server& operator= (const Server&) = delete;

    private:
        
        void AddClientSocket(int socket, SocketType st, bool isUpdate=true);
        void AddSocket (int socket);
        void DeleteSocket (int socket);
};
