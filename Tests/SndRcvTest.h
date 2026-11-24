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
#include <stop_token>

static void StartServer()
{
    try
    {
        Server server;
       
        auto ssocket = std::make_unique<ServerSocket> ("127.0.0.1", "ip4", 
                        2000, false, false);
        server.AddSocket(std::move(ssocket) );
        
        std::stop_token st;
        auto ctx = server.WaitClients(st);
        auto clientSocket = ctx->GetSocket();

      //  Log() << "descriptor " + std::to_string(clientSocket->Get()) << std::endl;
        clientSocket->SetTimeout();

        Buffer packet;
        auto [size, result] = clientSocket->ReadPacket(packet, MaxBufferSize);
        if (result<0)
        {
            perror ("Readpacket ");
            std::cout << "error packet read " << result << std::endl;
            return;
         }


        auto [_, result1] = clientSocket->WritePacket(packet, packet.size());
        if (result1<0)
        {
            perror ("Writepacket ");
            std::cout << "error packet write " << result1 << std::endl;
            return;
        }
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    catch (...)
    {
        std::cout << "Thread exception";
    }
}

static void TestSndRcv()
{
    std::cout << "START test SEND RECV" << std::endl;
    std::jthread thr (StartServer);

    sleep(5);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    ClientSocket cSocket(s);

    cSocket.Connect ("127.0.0.1", 2000);
    cSocket.SetTimeout();

    Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "49001");

    auto [_,result1] = cSocket.WritePacket(packetGet, packetGet.size());
    if (result1<0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        assert(!"Error write TestSndRcv");
        return;
    }

    Buffer packet;
    auto [_, result] = cSocket.ReadPacket(packet, MaxBufferSize);
    if (result<0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;

        assert(!"Error read TestSndRcv");
        return;
     }


    thr.join();
    assert (packet==packetGet); 
    std::cout << "End test" << std::endl;
}

static void TestConnect()
{
    std::cout << "Start test connect" << std::endl;
    std::thread thr (StartServer);

    sleep(1);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    {
        ClientSocket cSocket(s);
        cSocket.Connect ("127.0.0.1", 2000);

    }
    thr.join();
    std::cout << "End test" << std::endl;
}

