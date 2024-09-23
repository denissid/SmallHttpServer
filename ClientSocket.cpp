#include "ClientSocket.h"

#include <vector>
#include <iostream>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <stdio.h>

#include "Logger.h"
#include "HTTPPacket.h"

using namespace std;


ClientSocket::ClientSocket (int socket):m_socket(socket)
{
		Log() << "SOCKET " + to_string(m_socket) << std::endl;
}

void ClientSocket::SetTimeout ()
{ 
    timeval timeout = {0};
    timeout.tv_sec  = 5; 
    timeout.tv_usec = 0;
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
}

bool ClientSocket::Connect(const std::string& ip, int port)
{
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip.c_str());

    socklen_t addrlen = sizeof(addr);

    int r = connect (m_socket, (sockaddr*)&addr, addrlen);
    if (r<0)
    {
        LogError() << " error connection " + to_string(r) << std::endl;
        return false;
    }
    return true;
}

bool ClientSocket::IsAlive() const 
{
    fd_set sockets;
    FD_ZERO(&sockets);
    FD_SET(m_socket, &sockets);

    struct timeval  tv={1, 0};
    int r = select(m_socket+1, &sockets, NULL, NULL, &tv);
    return r>0;
}

int ClientSocket::ReadPacket(Buffer& buffer) const
{	
	buffer.resize(65535);
	
	int size = 0, offset = 0;
	do
	{
		offset += size;

        if (HTTPPacket::IsBlankLine(offset, buffer))
        {
            Log() << "stop reading from socket (found blank line) " << std::endl;
            break;
        }
		size  = recv (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
    }
	while ( size>0 );

    if ( size<=0 )
    {
        if (size<0)
            LogError() << " errno recv '" + to_string(errno) + "' size " + to_string(size) << std::endl;
        return size;
    }

	Log() << "Size packet '" + to_string(offset) + "' " << std::endl;
	buffer.resize (offset);
    return offset;
}

int ClientSocket::WritePacket (const Buffer& buffer) const 
{
	int size = 0, offset = 0;
	do
	{
		offset += size;
		size = send (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
	}
	while ( offset<buffer.size() && size>0);

	if ( size<0 )
	{
        LogError() << " error send " + to_string(errno) << std::endl;
        return size;
	}

    return offset;
}

void ClientSocket::CloseSocket()
{
	if (m_socket!=-1)
	{
		//Log() << "Client Socket Closed" << std::endl;
		shutdown(m_socket, SHUT_RDWR);
		close(m_socket);
		m_socket = -1;
	}
}

ClientSocket::~ClientSocket ()
{
	CloseSocket();
}

