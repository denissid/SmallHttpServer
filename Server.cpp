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


namespace 
{
	const int max_events = 300;
}

Server::Server (): m_epoll(-1), m_tlsContext(true)
{
	log("Create epoll descriptor");
	m_epoll = epoll_create1(0);
	if (m_epoll==-1)
	{
		throw NetException("Error epoll create");
	}
}

std::unique_ptr<Socket> Server::WaitClients(std::stop_token st)
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

        log("Count events ", countEvents);

		for ( size_t i = 0; i < countEvents; ++i)
		{
            int masterSocket = Events[i].data.fd;
            log("master socket ", masterSocket);
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
                    log("Connect to secure socket 8080 ");
                    Acceptor acceptor(s->Get(), &m_tlsContext);
                    return s->Accept(acceptor);
                }
                else
                {
                    log("Connect to  socket  2000 ");
                    Acceptor acceptor(s->Get());
                    return s->Accept(acceptor);
                }
                
			}
		}
	}
	while(!st.stop_requested());

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
		logError("epoll_ctl del", errno) ;
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
		logError("epoll_ctl add socket", errno);
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

