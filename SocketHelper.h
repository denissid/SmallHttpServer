#pragma once

#include "Logger.h"

#include <openssl/ssl.h>
#include <sys/socket.h>
#include <type_traits>

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
            Log() << "stop reading from socket (found blank line) " << std::endl;
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
            LogError() << " errno recv '" + std::to_string(errno) + "' size " + std::to_string(size) << std::endl;
        return size;
    }

	Log() << "Size packet '" + std::to_string(offset) + "' " << std::endl;
	buffer.resize (offset);
    return offset;
}

template <typename T>
int Write (const Buffer &buffer, T socket, int flags)
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
    LogError() << " error send " + std::to_string(errno) << std::endl;
        return size;
	}

    return offset;
}


