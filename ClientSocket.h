#pragma once 
#include <vector>
#include <string>

typedef std::string Buffer;

class ClientSocket
{
	    int m_socket;

	public:

		ClientSocket (int socket);

        bool Connect(const std::string& ip, int port);
        void SetTimeout ();

		Buffer ReadPacket() const;
		void WritePacket (const Buffer& packet) const;

		~ClientSocket ();

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator= (const ClientSocket&) = delete;

	private:
        
		void CloseSocket();
};


