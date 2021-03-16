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

ClientSocket::ClientSocket (int socket):m_socket(socket)
{
}

Buffer ClientSocket::ReadPacket() const
{	
    Buffer buffer;
	buffer.resize(65535);
	
	int size = 0, offset = 0;
	do
	{
		offset +=size;
		size  = recv (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
	}
	while (size > 0);

    //TODO EAGAIN is not processed
	if (size==-1 && errno!=EAGAIN)
	{
		Log() << "Error socket " << (int)errno << endl;
		perror("Error socket");
	}

	Log() << "Received size = " <<offset <<  endl;
	buffer.resize (offset);
    return buffer;
}

void ClientSocket::WritePacket (const Buffer& buffer)
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
	if (m_socket !=-1)
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

