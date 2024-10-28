#pragma once

#include "Logger.h"
#include "TLSContext.h"

#include "Socket.h"
#include "ClientSocket.h"
#include "TLSSocket.h"

#include <openssl/ssl.h>
#include <sys/socket.h>
#include <type_traits>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <span>
#include <string>
#include <memory>

class Acceptor 
{

        int m_socket = -1;
        TLSContext *m_context = nullptr;

    public:

        Acceptor (int masterSocket):m_socket(masterSocket)
        {
        }

        Acceptor (int masterSocket, TLSContext *context): m_socket(masterSocket), m_context(context)
        {
        }

        std::unique_ptr<Socket>  operator() () const;

        Acceptor(const Acceptor& a) = delete;
        Acceptor(Acceptor&& a) = delete;
        Acceptor& operator= (const Acceptor&) = delete;
        Acceptor& operator= (Acceptor&&) = delete;
};

template <typename T>
int Read (Buffer &buffer, T socket, int flags)
{	
    buffer.resize(65535);
	
	int size = 0, offset = 0;
	do
	{
		offset += size;
        if (HTTPPacket::IsBlankLine(offset, buffer))
        {
            log("stop reading from socket (reason blank line) ");
            break;
        }

        if constexpr( std::is_integral<T>::value )
            size = recv(socket, (void*)&buffer[offset], buffer.size()-offset, flags);
        else
            size = SSL_read((SSL*)socket, (void*)&buffer[offset], buffer.size()-offset);
    }
	while ( size>0 );

    if ( size<=0 )
    {
        if (size<0)
            logError(" errno recv '", errno, "' size (", size, ")");
        return size;
    }

	log("Size packet '", offset, "' ");
	buffer.resize (offset);
    return offset;
}

template <typename T>
int Write (std::span<const char> buffer, T socket, int flags)
{	
    int size = 0, offset = 0;
	do
	{
		offset += size;
        if constexpr( std::is_integral<T>::value )
		    size = send (socket, &buffer[offset], buffer.size()-offset, MSG_NOSIGNAL);
        else
            size = SSL_write(socket, &buffer[offset], buffer.size()-offset);
	}
	while ( offset<buffer.size() && size>0);

	if ( size<0 )
	{
        logError(" error send ", errno);
        return size;
	}

    return offset;
}


