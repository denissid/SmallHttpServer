#pragma once 
#include <vector>
#include <string>

typedef std::string Buffer;

class ClientSocket
{
	int m_socket;
	public:

		ClientSocket (int socket);

		void ReadPacket(Buffer& packet);
		void WritePacket (const Buffer& packet);

		~ClientSocket ();

	private:
		void CloseSocket();
};


