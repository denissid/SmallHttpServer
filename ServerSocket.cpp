#include "ServerSocket.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>


#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>

#include "Exceptions.h"
#include "Logger.h"
#include "Options.h"

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
        Log() << "flags " << flags << endl;
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
			LogError() << "set socket option " << errno;
		}
	}
}


ServerSocket::ServerSocket(const std::string& address, const std::string &family, int port, bool isBlock, bool isSecure): m_isSecure(isSecure)
{
    Log() << "Configure local address " << std::endl;
	Log() << "Create master socket " + address +  ":" + to_string(port) << std::endl;

	m_socket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
    Log() << "master socket " + to_string(m_socket) << std::endl;
	sockaddr_in sockAddr = {0};
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_port = htons(port);
    int f = family==ipFamily::ip4 ? AF_INET : AF_INET6;

	if (inet_pton(f, address.c_str(), &(sockAddr.sin_addr)) == 0)
	{
		throw NetException("Error convert ip address");
	}

	SetReuseSocket(m_socket);

	Log() << "Bind master socket" << std::endl;
	int errb = bind (m_socket, (sockaddr*) (&sockAddr), sizeof(sockAddr));
	if (errb==-1)
	{
	    perror("error ");
		throw NetException("BIND error");
	}
	
    if (!isBlock)
	    SetNonblock(m_socket);

	Log() << "Make master socket listened" << endl;
	listen (m_socket, SOMAXCONN);
}

int ServerSocket::Accept()
{	
    Log() << "Accept socket " << endl;

    sockaddr_storage clientAddress = {0};
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept (m_socket, (sockaddr*)&clientAddress, &clientLen);
    if (clientSocket<0)
    {
        LogError() << errno << std::endl;
    }
    char address_buffer[100]={0};
    getnameinfo((sockaddr*) &clientAddress, clientLen, address_buffer, sizeof(address_buffer),
            0, 0, NI_NUMERICHOST);

    string s(address_buffer);
    Log() << s << std::endl;

    return clientSocket;
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

