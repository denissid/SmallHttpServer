#pragma once 
#include "Buffer.h"
#include <tuple>

enum class SocketType
{
    Read,
    Write,
    None
};

class Socket
{
    public:
    
        virtual std::tuple<int, ssize_t> ReadPacket(Buffer& packet, size_t size) const = 0;
		virtual std::tuple<int, ssize_t>  WritePacket (const Buffer& packet, size_t size) const = 0;
        virtual void SetTimeout() = 0;
        virtual bool IsAlive() const = 0;
        virtual int Get() const = 0;

        virtual ~Socket() {}
};


