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
	private:
		Server(const Server&);
		Server& operator= (const Server&);
};
