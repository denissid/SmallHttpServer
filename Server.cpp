#include "Server.h"

#include <iostream>
#include <atomic>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

#include "Exceptions.h"
#include "Logger.h"
#include "ClientSocket.h"
#include "Options.h"

using namespace std;

extern std::atomic<bool> keepThreadRunning;

namespace 
{
	const int max_events = 300;
}

Server::Server (): m_epoll(-1)
{
	Log() << "Create epoll descriptor" <<endl;
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		throw NetException("Error epoll create");
	}
}

int Server::WaitClients()
{
	do
	{
		epoll_event Events[max_events] = {0};
		const size_t countEvents = epoll_wait(m_epoll, Events, max_events, -1);
		if (countEvents==-1)
		{
			perror("WaitClient ");
			throw NetException("Error epoll_wait create");
		}

        Log() << "Count events " + to_string(countEvents) << std::endl;

		for ( size_t i = 0; i < countEvents; ++i)
		{
			if (Events[i].data.fd==m_serverSocket.Get())
			{
				Log() << "Accept socket " << endl;

                sockaddr_storage client_address = {0};
                socklen_t client_len = sizeof(client_address);
				int clientSocket = accept (m_serverSocket.Get(), (sockaddr*)&client_address, &client_len);
                char address_buffer[100]={0};
                getnameinfo((sockaddr*) &client_address, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);

                string s(address_buffer);
                Log() << s << std::endl;

				//SetNonblock(clientSocket);
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
	while(keepThreadRunning);

    return -1;
}

void Server::DeleteSocket (int socket)
{	
    epoll_event event = {0};
	event.data.fd = socket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_DEL, socket, &event);

	if (err==-1)
	{
		LogError() << "epoll_ctl del " + to_string(errno) ;
	}

}

void Server::AddSocket(ServerSocket &s)
{
    m_serverSocket = std::move(s);
    AddSocket(m_serverSocket.Get());
}

void Server::AddSocket (int socket)
{	
    epoll_event event = {0};
	event.data.fd = socket;
	event.events = EPOLLIN;
	int err = epoll_ctl(m_epoll, EPOLL_CTL_ADD, socket, &event);

    if (err==-1)
	{
		LogError() << "epoll_ctl add socket" + to_string(errno) << std::endl;
		throw NetException("add socket in epoll");
	}
}

Server::~Server ()
{	
    DeleteSocket(m_serverSocket.Get());

}

