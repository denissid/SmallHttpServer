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

std::unique_ptr<IOContext> Server::WaitClients(std::stop_token st)
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
            int socket = Events[i].data.fd;
            log("master socket ", socket);
            auto it =  m_serverSockets.find(socket);

			if (it!=end(m_serverSockets))
			{
                    auto &s = it->second;
                    if (s->IsSecure())
                    {
                        log("Connect to secure socket 8080 ");
                        Acceptor acceptor(s->Get(), &m_tlsContext);
                        auto soc =  s->Accept(acceptor);
                        return std::make_unique<IOContext>(IOContext::Type::Read, std::move(soc));
                    }
                    else
                    {
                        log("Connect to  socket  2000 ");
                        Acceptor acceptor(s->Get());
                        int clientSocket  = acceptor.AcceptSocket();
                        if (clientSocket<0)
                        {
                            logError("client socket return error");
                        }
                        SetNonblock(clientSocket);
                        AddClientSocket(clientSocket, SocketType::Read, false);
                        //return acceptor(); //s->Accept(acceptor);
                    }
            }
            else
            {
                auto s = m_contexts.find(socket); 
                if (s==end(m_contexts))
                {
                    log("Create client socket and context ");
                    auto soc = std::make_unique<ClientSocket>(socket);
                    return std::make_unique<IOContext> (IOContext::Type::Read, std::move(soc));
                }
                else
                {
                    log("use already created client socket ");
                    auto client = move(s->second);
                    m_contexts.erase(s);
                    return move(client);
                }
		    }
        }
	}
	while(!st.stop_requested());

    logError("Wait clients returned 0");
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

void Server::AddSocket(std::unique_ptr<ServerSocket> s)
{
    AddSocket(s->Get());
    m_serverSockets.insert({s->Get(), std::move(s)});
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

void Server::AddSocket(SocketType st, bool isUpdate, std::unique_ptr<IOContext> context)
{
    int lsoc = context->GetSocket()->Get();
    AddClientSocket(lsoc, st, isUpdate);
    //m_sockets.insert({socket->Get(), std::move(socket)});
    m_contexts.insert({lsoc, std::move(context)});
}

void Server::AddClientSocket(int socket, SocketType st, bool isUpdate)
{ 
    if (st==SocketType::None)
        throw NetException ("Unknown socket type");

    epoll_event event = {0};
	event.data.fd = socket;

    int flag = EPOLLIN; 
    if (st==SocketType::Write)
       flag |= EPOLLOUT;

	event.events = flag | EPOLLET| EPOLLONESHOT;
	int err = epoll_ctl(m_epoll, isUpdate ? EPOLL_CTL_MOD : EPOLL_CTL_ADD, socket, &event);
    if (err==-1)
	{
		logError("epoll_ctl add non blocksocket", errno);
		throw NetException("add socket in epoll");
	}

}

Server::~Server ()
{	
    for_each(begin(m_serverSockets), end(m_serverSockets), 
            [this] (auto &s)
            {
                DeleteSocket(s.second->Get());
            }
        );

}

