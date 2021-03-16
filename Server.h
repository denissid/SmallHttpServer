#pragma one

#include <string>
#include "ClientSocket.h"

class Server 
{
	    int m_masterSocket;
	    int m_epoll;
	
	public:
        
		Server (const std::string& address, int port);
		int WaitClients();
		~Server ();

		Server(const Server&) = delete;
		Server& operator= (const Server&) = delete;
};
