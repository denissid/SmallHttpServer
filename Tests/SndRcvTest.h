#pragma once 

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "Server.h"
#include "ServerSocket.h"
#include "Logger.h"

#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>

#include <thread>
#include <atomic>

extern std::atomic<bool> keepThreadRunning;

static void StartServer()
{
    keepThreadRunning = true;
    Server server;
   
    ServerSocket ssocket("127.0.0.1", "ip4", 
                    2000, false, false);
    server.AddSocket(ssocket);
    
    int s = server.WaitClients();

    Log() << "descriptor " + std::to_string(s) << std::endl;
    ClientSocket cSocket(s);
    cSocket.SetTimeout();

    Buffer packet;
    int result = cSocket.ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        assert(!"Read packet");
        return;
     }


    int result1 = cSocket.WritePacket(packet);
    if (result1<=0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        assert (!"Erro Write packet");
        return;
    }

}

static void TestSndRcv()
{
    std::thread thr (StartServer);

    sleep(1);
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

    

    thr.join();

    assert (packet==packetGet); 
    //std::cout << packet << std::endl;
}


