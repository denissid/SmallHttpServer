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

static void StartServerEAGAIN()
{
    try 
    {
        Server server;
       
        auto ssocket = std::make_unique<ServerSocket> ("127.0.0.1", "ip4", 
                        2000, false, false);
        server.AddSocket(std::move(ssocket));
        Buffer packet;
        int result = 0;

        std::stop_token st;
        do {
            auto ctx = server.WaitClients(st);
            auto clientSocket = ctx->GetSocket();
            
            std::cout << "Read packet" << std::endl;
            auto [size, res] = clientSocket->ReadPacket(packet, MaxBufferSize);
            if (res==0)
            {
                std::cout << "exit breakout res==0" << std::endl;
                break;
            }
            else
            if (res<0)
            {
                std::cout << "Error " << std::endl;
                std::cout << res << std::endl;
                std::cout << packet << std::endl;
                break;
            }
            else
            if (errno==EAGAIN)
            {
               std::cout << packet << std::endl;
               result +=  res;
               std::cout << result << std::endl;
               if (result>=300)
               {
                    std::cout << "exit alldownload" << std::endl;
                    break;
               }
               server.AddSocket(SocketType::Read, true, std::move(ctx));
            }
        }
        while(1);
    }
    catch(...)
    {
        std::cout << "Unknon exception";
    }

    std::cout << "exit server" << std::endl;
}

static void TestEAGAINWrite()
{ 
    std::cout << "Start test EAGAIN Write" << std::endl;
    std::jthread thr (StartServerEAGAIN);

    sleep(2);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    {
        ClientSocket cSocket(s);
        cSocket.Connect ("127.0.0.1", 2000);

        Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "49001");

        int size = packetGet.size();
        std::cout << "send packet with size " << size << std::endl; 
        int sizeTail = size/3;
        std::cout << "send buffer with size " << sizeTail << std::endl;

        int pos = 0;
        do
        { 
            auto part = packetGet.substr(pos, sizeTail);
            auto [_,result] = cSocket.WritePacket(part, sizeTail);
      
            if (result>0)
            {
                std::cout << "sended size " << result << std::endl;
                pos+=result;
                if (pos+sizeTail>size)
                {
                    sizeTail = size-pos;
                }
            }
            else
            if (result==0)
            {
                break;
            }
            else
            if (result<0)
            {
                perror ("Writepacket (send EAGAIN) ");
                break;
            }
            usleep(500000);
        }
        while(1);

        std::cout << "wain join server" << std::endl;
    }
    thr.join();
    std::cout << "End test" << std::endl;
}

namespace 
{
    Buffer CreatePacket()
    {
        Buffer packet; 
        for (int i =0; i<3*1023*1023; ++i)
        {
            packet+='1';
        }
        return packet;
    }
}

static void StartServerEAGAIN2()
{
    Server server;
   
    auto ssocket = std::make_unique<ServerSocket> ("127.0.0.1", "ip4", 
                    2000, false, false);
    server.AddSocket(std::move(ssocket));

    int result = 0;

    std::stop_token st;

    Buffer packetS = CreatePacket();
    size_t size = packetS.size();


    int numEAGAIN = 0;
    int pos = 0;

    do {
        auto context = server.WaitClients(st);
        auto clientSocket = context->GetSocket();

        if (context->GetType()==IOContext::Type::Read)
        {
            auto [sizer, res] = clientSocket->ReadPacket(context->GetRefBuffer(), MaxBufferSize);
            if (res<0)
            {
                logError ("exit breakout res<0");
                break;
            }
            else
            if (res==0)
            {
                logError ("End reading");
                logError ("Change type of context");
                context->SetType(IOContext::Type::Write);
                server.AddSocket(SocketType::Read, true, std::move(context));

            }
            else
            if (errno==EAGAIN)
            {
                    std::cout << "exit alldownload" << context->GetRefBuffer() << std::endl;
                    context->SetSize(sizer+context->GetSize());
                    context->SetType(IOContext::Type::Read);
                    result = 0;
                    server.AddSocket(SocketType::Read, true, std::move(context) );
                    continue;
            }
        }

        int sndbuf = 2024;
        setsockopt(clientSocket->Get(), SOL_SOCKET, SO_SNDBUF, &sndbuf, sizeof(sndbuf));
        std::cout << "Write packet" << std::endl;
        auto [sz,res] = clientSocket->WritePacket(packetS, size);
        if (res<0 && errno!=EAGAIN)
        {
            std::cout << "exit breakout res==0 " << res << "send " << sz << std::endl;
            break;
        }
        else
        if (sz==size)
        {
            std::cout << "exit  " << sz << std::endl;
            assert(!"ERROR");
            break;
        }
        if (errno==EAGAIN)
        {
           logError("EAGAIN ");
           numEAGAIN++;
           if (numEAGAIN>2)
           {
               logError("Get 2 times EGAIN on Write");
               break;
           }

           context->SetType(IOContext::Type::Write);
           server.AddSocket(SocketType::Write, true, std::move(context));
        }
    }
    while(1);

    std::cout << "exit server" << std::endl;
}


static void TestEAGAINRead()
{   
   std::cout << "Start test EAGAIN Read" << std::endl;
   std::jthread thr (StartServerEAGAIN2);

    sleep(2);
    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    {
        ClientSocket cSocket(s);
        cSocket.Connect ("127.0.0.1", 2000);

        Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "49001");

        int size = packetGet.size();
        int pos = 0;
        do
        { 
            auto [_,result] = cSocket.WritePacket(packetGet, size);
            if (result>0)
            {
                std::cout << "sended size " << result << std::endl;
                if (result>=packetGet.size())
                    break;
            }
            else
            if (result<=0)
            {
                perror ("Writepacket (send EAGAIN) ");
                break;
            }
        }
        while(1);

        int res = 0, size1 = 0;
        Buffer p;
        do
        {
            Buffer packet;
            auto [sizeR,res1] = cSocket.ReadPacket(packet, 3000);
            res = res1;
            size1 = sizeR;
            p += packet;
        }
        while(size1>0);

        std::cout << "received " << p.size()  ;
    }

    thr.join();
}
