#pragma one

#include <string>

class Server 
{
	    int m_masterSocket;
	    int m_epoll;
	
	public:
        
		Server (const std::string& address, const std::string &family, int port);
		int WaitClients();

		~Server ();

		Server(const Server&) = delete;
		Server& operator= (const Server&) = delete;

    private:
        
        void AddSocket (int socket);
        void DeleteSocket (int socket);
};
