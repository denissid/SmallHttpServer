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
			cout << "set socket option " << errno << endl;
		}
	}

	const int max_events = 300;
}

Server::Server (const std::string& address, int port): m_masterSocket(-1), m_epoll(-1)
{
	cout << "Create master socket " << address <<":"<< port << endl;
	m_masterSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in sockAddr;
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
	if (inet_pton(AF_INET, address.c_str(), &(sockAddr.sin_addr)) == 0)
	{
		throw NetException("Error convert ip address");
	}

	SetReuseSocket(m_masterSocket);

	cout << "Bind master socket" << endl;
	int errb = bind (m_masterSocket, (sockaddr*) (&sockAddr), sizeof(sockAddr));
	if (errb==-1)
	{
		throw NetException("BIND error");
	}
	
	SetNonblock(m_masterSocket);

	cout << "Make master socket listened" << endl;
	listen (m_masterSocket, SOMAXCONN);
	
	cout << "Create epoll descriptor" << endl;
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		throw NetException("Epoll create");
	}
	
	cout << "Added master socket in epoll" << endl;
	epoll_event event;
	event.data.fd = m_masterSocket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_ADD, m_masterSocket, &event);
	if (err==-1)
	{
		cout << "epoll_ctl" << errno << endl;
		throw NetException("Epoll_ctl create");
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
			throw NetException("Epoll_wait create");
			return -1;
		}

		for ( size_t i = 0; i<countEvents; ++i)
		{
			if (Events[i].data.fd == m_masterSocket)
			{
				cout << "Accept socket" << endl;
				int slaveSocket = accept (m_masterSocket, 0, 0);
				SetNonblock(slaveSocket);
				epoll_event slaveEvent;
				slaveEvent.data.fd = slaveSocket;
				slaveEvent.events = EPOLLIN;
				epoll_ctl(m_epoll, EPOLL_CTL_ADD, slaveSocket, &slaveEvent);
			}
			else
			{	
				epoll_event slaveEvent;
				slaveEvent.data.fd = Events[i].data.fd;
				slaveEvent.events = EPOLLIN;

				epoll_ctl(m_epoll, EPOLL_CTL_DEL, Events[i].data.fd, &slaveEvent);
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

