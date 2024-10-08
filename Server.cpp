#include "Server.h"

#include <iostream>
#include <atomic>
#include <algorithm>

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
#include <future>

#include "Exceptions.h"
#include "Logger.h"
#include "ClientSocket.h"
#include "Options.h"

using namespace std;

//TODO need refactoring 
extern std::atomic<bool> keepThreadRunning;

namespace 
{
	const int max_events = 300;
}

Server::Server (): m_epoll(-1), m_tlsContext(true)
{
	Log() << "Create epoll descriptor" <<endl;
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		throw NetException("Error epoll create");
	}
}

std::unique_ptr<Socket> Server::WaitClients()
{
	do
	{
       // Log() << "WAIT " << std::endl;
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
            int masterSocket = Events[i].data.fd;
            Log() << "master socket " + to_string(masterSocket) << std::endl;
            auto s = std::find_if( begin(m_serverSockets),
                                   end(m_serverSockets),
                                   [masterSocket] (const ServerSocket &s)
                                   {
                                        return s.Get()==masterSocket;
                                   });
                          
			if (s!=end(m_serverSockets))
			{
                if (s->IsSecure())
                {
                    Log() << "Connect to secure socket 8080 " << std::endl;
                    Acceptor acceptor(s->Get(), &m_tlsContext);
                    return s->Accept(acceptor);
                }
                else
                {
                    Log() << "Connect to  socket  2000 " << std::endl;
                    Acceptor acceptor(s->Get());
                    return s->Accept(acceptor);
                }
                
			}
		}
	}
	while(keepThreadRunning);

    return nullptr;
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
    AddSocket(s.Get());
    m_serverSockets.push_back(std::move(s));
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
    for_each(begin(m_serverSockets), end(m_serverSockets), 
            [this] (const ServerSocket &s)
            {
                DeleteSocket(s.Get());
            }
        );

}

