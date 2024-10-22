#pragma once 
#include <string>

typedef std::string Buffer;

class Socket
{
    public:
    
        virtual int ReadPacket(Buffer& packet) const = 0;
		virtual int WritePacket (const Buffer& packet) const = 0;
        virtual void SetTimeout() = 0;
        virtual bool IsAlive() const = 0;
        virtual ~Socket() {}
};

