#include "ClientSocket.h"

#include <vector>
#include <iostream>

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#include "Logger.h"

using namespace std;

namespace
{
    bool IsEndPacket(const int offset, const Buffer &buffer)
    { 
        string endPacket = "\r\n\r\n";
        auto eSize = endPacket.size();
        if (offset>=eSize)
        {
            auto l = buffer.substr(offset-eSize, eSize);
            return l==endPacket;
        }
        return false;
    }
}

ClientSocket::ClientSocket (int socket):m_socket(socket)
{
    timeval timeout = {0};
    timeout.tv_sec  = 5; 
    timeout.tv_usec = 0;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

Buffer ClientSocket::ReadPacket() const
{	
    Buffer buffer;
	buffer.resize(65535);
	
	int size = 0, offset = 0;
	do
	{
		offset += size;

        if (IsEndPacket(offset, buffer))
        {
            Log() << "stop reading from socket" << std::endl;
            break;
        }
		size  = recv (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);

    }
	while (size > 0);

    //TODO EAGAIN is not processed
	if (size==-1 && errno==EAGAIN)
	{
		Log() << "Error socket TIMEOUT--> " << (int)errno << endl;
		perror("Error socket");

        buffer.clear();
        return buffer;
	}

	Log() << "Received offset = " <<offset <<  endl;
	buffer.resize (offset);
    return buffer;
}

void ClientSocket::WritePacket (const Buffer& buffer) const 
{
	int size = 0, offset = 0;
	do
	{
		offset += size;
		size = send (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
	}
	while(offset<buffer.size() && size>0);

    //TODO EAGAIN is not processed
	if (size==-1)
	{
		Log() << "Error socket " << (int)errno << endl;
	}
}

void ClientSocket::CloseSocket()
{
	if (m_socket!=-1)
	{
		Log() << "Client Socket Closed" << std::endl;
		shutdown(m_socket, SHUT_RDWR);
		close(m_socket);
		m_socket = -1;
	}
}

ClientSocket::~ClientSocket ()
{
	CloseSocket();
}

