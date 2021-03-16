#pragma once 
#include <vector>
#include <string>

typedef std::string Buffer;

class ClientSocket
{
	    int m_socket;

	public:

		ClientSocket (int socket);

		Buffer ReadPacket() const;
		void WritePacket (const Buffer& packet);

		~ClientSocket ();

        ClientSocket(const ClientSocket&) = delete;
        ClientSocket& operator= (const ClientSocket&) = delete;

	private:
        
		void CloseSocket();
};


