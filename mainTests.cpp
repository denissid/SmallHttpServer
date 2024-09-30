#include <iostream>

#include "Tests/Tests.h"
#include "Tests/TLSTests.h"

#include "ClientSocket.h"
#include "HTTPPacket.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

#include <thread>
#include <atomic>


void TestSndRcv()
{ 
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ClientSocket cSocket(s);

    cSocket.Connect ("127.0.0.1", 2000);
    cSocket.SetTimeout();

    Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "49001");

    int result1 = cSocket.WritePacket(packetGet);
    if (result1<=0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        assert (!"Erro Write packet");
        return;
    }

    Buffer packet;
    int result = cSocket.ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        assert(!"Read packet");
        return;
     }

    //std::cout << packet << std::endl;
}

int main (int argc, char** argv)
{
    //TestExtract();
	//TestGETParse();
	//TestSplit();
    //TestGetContentType();

//    TestSndRcv();
    //TestTLSCreating();
    
    TestTLSSnd();

	return 0;
}
