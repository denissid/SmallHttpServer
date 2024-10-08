#pragma once 
#include <vector>
#include <string>

#include "Socket.h"

class ClientSocket: public Socket
{
	    int m_socket;

	public:

		ClientSocket (int socket);

        bool Connect(const std::string& ip, int port);
        void SetTimeout ();
		int ReadPacket(Buffer& packet) const;
		int WritePacket (const Buffer& packet) const;
        bool IsAlive() const;
        int Get() const
        {
            return m_socket;
        }

		~ClientSocket ();

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator= (const ClientSocket&) = delete;

	private:
        
		void CloseSocket();
};


