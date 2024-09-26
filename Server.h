#pragma one

#include <string>
#include "ServerSocket.h"

class Server 
{
	    int m_epoll;
        ServerSocket m_serverSocket;
	
	public:
        
		Server ();
		~Server ();

		int WaitClients();
        void CreateEpoll();
        void AddSocket(ServerSocket &s);
        void AddSocket (int socket);

		Server(const Server&) = delete;
		Server& operator= (const Server&) = delete;

    private:
        
        void DeleteSocket (int socket);
};
