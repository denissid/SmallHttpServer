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

using namespace std;

namespace 
{

	int set_nonblock(int fd)
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

	const int max_events = 300;
}

Server::Server (const std::string& address, int port): m_masterSocket(-1), m_epoll(-1)
{
	cout << "Create master socket" << address <<":"<< port << endl;
	m_masterSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	//sockAddr.sin_addr.s_addr 
	if (inet_pton(AF_INET, address.c_str(), &(sockAddr.sin_addr)) == 0)
	{
		cout << "Error conver ip address" << endl;
	}

	cout << "Bind master socket" << endl;
	int errb  = bind (m_masterSocket, (sockaddr*) (&sockAddr), sizeof(sockAddr));
	if (errb==-1)
	{
		perror("BIND ");
	}
	
	set_nonblock(m_masterSocket);

	cout << "Make master socket listened" << endl;
	listen (m_masterSocket, SOMAXCONN);
	
	cout << "Create epoll descriptor" << endl;
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		cout << "epoll create " << errno << endl;
	}
	
	cout << "Added master socket in epoll" << endl;
	epoll_event event;
	event.data.fd = m_masterSocket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_ADD, m_masterSocket, &event);
	if (err==-1)
	{
		cout << "epoll_ctl" << errno << endl;
	}
}

int Server::WaitClients()
{
	do
	{
		epoll_event Events[max_events];
		const size_t countEvents = epoll_wait(m_epoll, Events, max_events, -1);
		if (countEvents==-1)
		{
			perror("WaitClient ");
			return -1;
		}

		for ( size_t i = 0; i<countEvents; ++i)
		{
			if (Events[i].data.fd == m_masterSocket)
			{
				cout << "Accept socket" << endl;
				int slaveSocket = accept (m_masterSocket, 0, 0);
				set_nonblock(slaveSocket);
				epoll_event slaveEvent;
				slaveEvent.data.fd = slaveSocket;
				slaveEvent.events = EPOLLIN;
				epoll_ctl(m_epoll, EPOLL_CTL_ADD, slaveSocket, &slaveEvent);
			}
			else
			{
				cout << "Start read slave socket" << endl;
				return Events[i].data.fd;
			}
		}
	}
	while(1);
}

Server::~Server ()
{	
	epoll_event event;
	event.data.fd = m_masterSocket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_DEL, m_masterSocket, &event);
	if (err==-1)
	{
		cout << "epoll_ctl del" << errno << endl;
	}

	cout << "Master socket closed" << endl;
	close (m_masterSocket);
}
