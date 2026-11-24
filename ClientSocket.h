#pragma once 
#include <vector>
#include <string>
#include <tuple>

#include "Socket.h"

class ClientSocket: public Socket
{
	    int m_socket;

	public:

		ClientSocket (int socket);

        bool Connect(const std::string& ip, int port);
        void SetTimeout ();
		std::tuple<int, ssize_t> ReadPacket(Buffer& packet, size_t size) const;
		std::tuple<int, ssize_t> WritePacket (const Buffer& packet, size_t size) const;
        bool IsAlive() const;
        int Get() const
        {
            return m_socket;
        }

		~ClientSocket ();

        ClientSocket(ClientSocket &&s)
        {
            m_socket = s.m_socket;
            s.m_socket = -1;
        }

        ClientSocket& operator= (ClientSocket &&s)
        {
            m_socket = s.m_socket;
            s.m_socket = -1;
            return *this;
        }

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator= (const ClientSocket&) = delete;

	private:
        
		void CloseSocket();
};


