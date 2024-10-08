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
    
    auto clientSocket = server.WaitClients();

  //  Log() << "descriptor " + std::to_string(clientSocket->Get()) << std::endl;
    clientSocket->SetTimeout();

    Buffer packet;
    int result = clientSocket->ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        return;
     }


    int result1 = clientSocket->WritePacket(packet);
    if (result1<=0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        return;
    }
}

static void TestSndRcv()
{
    std::thread thr (StartServer);

    sleep(5);
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
        return;
    }

    Buffer packet;
    int result = cSocket.ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        return;
     }


    thr.join();
    assert (packet==packetGet); 
    std::cout << "End test" << std::endl;
}

static void TestConnect()
{
    std::thread thr (StartServer);

    sleep(1);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ClientSocket cSocket(s);
    cSocket.Connect ("127.0.0.1", 2000);

    thr.join();
    std::cout << "End test" << std::endl;
}


