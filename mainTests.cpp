#include <iostream>

#include "Tests.h"
#include "ClientSocket.h"
#include "HTTPPacket.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

int main (int argc, char** argv)
{

    TestExtract();
	TestGETParse();
	TestSplit();

    {
        int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        ClientSocket cSocket(s);

        cSocket.Connect ("192.168.122.1", 49001);

        Buffer b = HTTPRequest::CreateGET("127.0.0.1", "49001");
        int i=10;
        do
        {
            cSocket.WritePacket(b);
            Buffer packet = cSocket.ReadPacket();
            std::cout << packet;

        } while(--i);

    }
	return 0;
}
