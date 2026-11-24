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
        TLSContext *m_tlsContext = nullptr;

    public:

        Acceptor (int masterSocket):m_socket(masterSocket)
        {
        }

        Acceptor (int masterSocket, TLSContext *context): m_socket(masterSocket), m_tlsContext(context)
        {
        }

        std::unique_ptr<Socket>  operator() () const;
        int AcceptSocket() const;

        Acceptor(const Acceptor& a) = delete;
        Acceptor(Acceptor&& a) = delete;
        Acceptor& operator= (const Acceptor&) = delete;
        Acceptor& operator= (Acceptor&&) = delete;
};

template <typename T>
std::tuple<int, ssize_t> Read (Buffer &buffer, size_t s, T socket, int flags)
{
    buffer.resize(s); //TODO fix it

	int res = 0, offset = 0;
	do
	{
		offset += res;
        if (HTTPPacket::IsBlankLine(offset, buffer))
        {
            log("stop reading from socket (reason blank line) ");
            break;
        }

        if constexpr( std::is_integral<T>::value )
            res = recv(socket, (void*)&buffer[offset], buffer.size()-offset, flags);
        else
            res = SSL_read((SSL*)socket, (void*)&buffer[offset], buffer.size()-offset);
    }
	while (res>0 && offset<buffer.size());

    if (res<0)
    {
        logError(" errno recv '", errno, "' res (", res, ")");

        return std::make_tuple(offset,res);
    }

	buffer.resize (offset);
    return std::make_tuple(offset, res);
}

template <typename T>
std::tuple<int, ssize_t>  Write (std::span<const char> buffer, size_t bsize, T socket, int flags)
{	
    int res = 0, offset = 0;
	do
	{
		offset += res;

        if constexpr( std::is_integral<T>::value )
		    res = send (socket, &buffer[offset], bsize-offset, MSG_NOSIGNAL);
        else
            res = SSL_write(socket, &buffer[offset], buffer.size()-offset);
	}
	while ( offset<buffer.size() && res>0);

	if (res<0)
	{
        logError(" error send ", errno);
        return std::make_tuple(offset, res);
	}

    return std::make_tuple(offset, res);
}

int SetNonblock(int fd);
int SetBlock(int fd);


