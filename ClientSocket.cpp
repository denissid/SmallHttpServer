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


using namespace std;

ClientSocket::ClientSocket (int socket):m_socket(socket)
{
}

void ClientSocket::ReadPacket(Buffer& buffer)
{	
	buffer.resize(1000);
	
	int size = 0, offset = 0;
	do
	{
		offset +=size;
		size  = recv (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
	//	cout << " SIZE " << size <<" OFFSET "<< offset << endl;
	}
	while (size > 0);

	if ( /*(size == 0) && */(errno != EAGAIN) )
	{
		CloseSocket();
	}
	else
	{
		buffer.resize (offset);
		//cout << (char*)&buffer[0];
	}
}

void ClientSocket::WritePacket (Buffer& buffer)
{
	int size = 0, offset = 0;
	do
	{
		offset += size;
		size = send (m_socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
	//	cout << "Size send" << buffer << size << endl;
	}
	while(offset<buffer.size() && size>0);
}

void ClientSocket::CloseSocket()
{
	if (m_socket !=-1)
	{
		std::cout << "Client Socket Closed" << std::endl;
		shutdown(m_socket, SHUT_RDWR);
		close(m_socket);
		m_socket = -1;
	}

}

ClientSocket::~ClientSocket ()
{
	CloseSocket();
}


