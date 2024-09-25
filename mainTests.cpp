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

std::atomic <int> cTimeout{0};
std::atomic <int> cThreads{0};

void SndRcvClient()
{ 
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ClientSocket cSocket(s);

    cSocket.Connect ("127.0.0.1", 2000);
    cSocket.SetTimeout();

    Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "49001");
    int i=10;
    do
    {
        int result1 = cSocket.WritePacket(packetGet);
        if (result1<=0)
        {
            perror ("Writepacket ");
            cTimeout++;
            std::cout << "error packet write " << result1 << std::endl;
            break;
        }

        Buffer packet;
        int result = cSocket.ReadPacket(packet);
        if (result<=0)
        {
            perror ("Readpacket ");
            cTimeout++;
            std::cout << "error packet read " << result << std::endl;
            break;
         }
        //std::cout << packet;
    } while(--i);

    cThreads++;
    std::cout << "STOP THREAD " << cThreads << std::endl;
}

int main (int argc, char** argv)
{
    TestExtract();
	TestGETParse();
	TestSplit();
    TestGetContentType();

    TestTLSCreating();

    return 0;
    const int max=800;
    int i=5, z=max;
    do 
    {
        std::thread thr (SndRcvClient);
        thr.detach();
        if (z<=0)
        {
            sleep(1);
            z=max;
        }
        z--;
    }while(--i);

    sleep(2);
    std::cout << "timeout " <<cTimeout << std::endl;
    std::cout << "closed " <<cThreads;
	return 0;
}
