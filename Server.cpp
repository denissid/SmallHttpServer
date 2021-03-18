#include "Server.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

#include "Exceptions.h"
#include "Logger.h"

using namespace std;

namespace 
{

	int SetNonblock(int fd)
	{
		int flags=0;
	#if defined(O_NONBLOCK)
		if (-1==(flags = fcntl(fd, F_GETFL, 0)))
			flags = 0;
		return fcntl (fd, F_SETFL, flags| O_NONBLOCK);
	#else
		flags = 1;
		return ioctl(fd, FIOBIO, &flags);
	#endif
	}

	void SetReuseSocket(int fd)
	{
		int reuse = 1;
		int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		if (reuse<0)
		{
			Log() << "set socket option " << errno << endl;
		}
	}

	const int max_events = 300;
}

Server::Server (const std::string& address, int port): m_masterSocket(-1), m_epoll(-1)
{
	Log() << "Create master socket " << address <<":"<< port << endl;
	m_masterSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr = {0};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &(sockAddr.sin_addr)) == 0)
	{
		throw NetException("Error convert ip address");
	}

	SetReuseSocket(m_masterSocket);

	Log() << "Bind master socket" << endl;
	int errb = bind (m_masterSocket, (sockaddr*) (&sockAddr), sizeof(sockAddr));
	if (errb==-1)
	{
		throw NetException("BIND error");
	}
	
	//SetNonblock(m_masterSocket);

	Log() << "Make master socket listened" << endl;
	listen (m_masterSocket, SOMAXCONN);
	
	Log() << "Create epoll descriptor" << endl;
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		throw NetException("Epoll create");
	}
	
	Log() << "Added master socket in epoll" << endl;

    AddSocket(m_masterSocket);

}

int Server::WaitClients()
{
	do
	{
		epoll_event Events[max_events]={0};
		const size_t countEvents = epoll_wait(m_epoll, Events, max_events, -1);
		if (countEvents==-1)
		{
			perror("WaitClient ");
			throw NetException("Epoll_wait create");
		}

        std::cout << "Count events" << countEvents << std::endl;

		for ( size_t i = 0; i<countEvents; ++i)
		{
			if (Events[i].data.fd == m_masterSocket)
			{
				Log() << "Accept socket" << endl;
				int clientSocket = accept (m_masterSocket, 0, 0);
				SetNonblock(clientSocket);

                AddSocket(clientSocket);
			}
			else
			{	
                DeleteSocket(Events[i].data.fd);
                        
				Log() << "Start read slave socket" << endl;
				return Events[i].data.fd;
			}
		}
	}
	while(1);
}

void Server::DeleteSocket (int socket)
{	
    epoll_event event = {0};
	event.data.fd = socket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_DEL, socket, &event);

	if (err==-1)
	{
		Log() << "epoll_ctl del" << errno << endl;
	}

}

void Server::AddSocket (int socket)
{	
    epoll_event event={0};
	event.data.fd = socket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_ADD, socket, &event);

    if (err==-1)
	{
		Log() << "epoll_ctl add socket" << errno << endl;
		throw NetException("Epoll_ctl create");
	}
}

Server::~Server ()
{	
    DeleteSocket(m_masterSocket);

	Log() << "Master socket closed" << endl;
	close (m_masterSocket);
}

