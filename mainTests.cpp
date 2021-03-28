#include <iostream>

#include "Tests.h"
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

    cSocket.Connect ("192.168.122.1", 49001);
    cSocket.SetTimeout();

    Buffer b = HTTPRequest::CreateGET("127.0.0.1", "49001");
    int i=10;
    do
    {
        cSocket.WritePacket(b);
        Buffer packet = cSocket.ReadPacket();
        if (packet.empty())
        {
            cTimeout++;
            std::cout << "Packet empty " << std::endl;
            break;
         }
    } while(--i);

    cThreads++;
    std::cout << "STOP THREAD " << cThreads << std::endl;
}

int main (int argc, char** argv)
{
    TestExtract();
	TestGETParse();
	TestSplit();

    const int max=800;
    int i=2000, z=max;
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
