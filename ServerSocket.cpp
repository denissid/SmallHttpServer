#include "ServerSocket.h"

#include "Exceptions.h"
#include "Logger.h"
#include "Options.h"
#include "Socket.h"
#include "ClientSocket.h"
#include "TLSSocket.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

using namespace std;
namespace 
{

	int SetNonblock(int fd)
	{
		int flags = 0;
	#if defined(O_NONBLOCK)
		if (-1==(flags = fcntl(fd, F_GETFL, 0)))
			flags = 0;
		return fcntl (fd, F_SETFL, flags| O_NONBLOCK);
	#else
		flags = 1;
		return ioctl(fd, FIOBIO, &flags);
	#endif
	}

	int SetBlock(int fd)
	{
		int flags = 0;
	#if defined(O_NONBLOCK)
		if (-1==(flags = fcntl(fd, F_GETFL, 0)))
			flags = 0;
        flags &= ~O_NONBLOCK;
        log( "flags ", flags);
		return fcntl (fd, F_SETFL, flags);
	#else
		return ioctl(fd, FIOBIO, &flags);
	#endif
	}


	void SetReuseSocket(int fd)
	{
		int reuse = 1;
		int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
		if (reuse<0)
		{
			logError("set socket option ", errno);
		}
	}
}


ServerSocket::ServerSocket(const std::string& address, const std::string &family, int port, bool isBlock, bool isSecure): m_isSecure(isSecure)
{
    log("Configure local address ");
	log("Create master socket ", address, ":", port);

	m_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    log("master socket ",m_socket);
	sockaddr_in sockAddr = {0};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
    int f = family==ipFamily::ip4 ? AF_INET : AF_INET6;

	if (inet_pton(f, address.c_str(), &(sockAddr.sin_addr)) == 0)
	{
		throw NetException("Error convert ip address");
	}

	SetReuseSocket(m_socket);

	log("Bind master socket");
	int errb = bind (m_socket, (sockaddr*) (&sockAddr), sizeof(sockAddr));
	if (errb==-1)
	{
	    perror("error ");
		throw NetException("BIND error");
	}
	
    if (!isBlock)
	    SetNonblock(m_socket);

	log("Make master socket listened");
	listen (m_socket, SOMAXCONN);
}

std::unique_ptr<Socket> ServerSocket::Accept(const Acceptor& acceptor)
{	
    log("Accept socket ");

    return acceptor();
}

int ServerSocket::Get() const
{
    return m_socket;
}

bool ServerSocket::IsSecure() const
{
    return m_isSecure;
}

ServerSocket::~ServerSocket()
{
    if (m_socket>=0)
	    close (m_socket);
}

ServerSocket& ServerSocket::operator= (ServerSocket&& t)
{
    m_socket = std::move(t.m_socket);
    m_isSecure = std::move(t.m_isSecure);
    t.m_socket = -1;
    t.m_isSecure = false;
    return *this;
}

ServerSocket::ServerSocket (ServerSocket&& t)
{
    m_socket = std::move(t.m_socket);
    m_isSecure = std::move(t.m_isSecure);
    t.m_socket = -1;
    t.m_isSecure = false;
}

