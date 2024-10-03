#include "TLSSocket.h"
#include "ServerSocket.h"
#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "Server.h"

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <thread>
#include <atomic>


extern std::atomic<bool> keepThreadRunning;
void TestTLSCreating()
{
    TLSSocket tls;
}


static void StartTLSServer()
{ 
    keepThreadRunning = true;
    TLSContext context(true);
    Server server;
   
    ServerSocket ssocket("127.0.0.1", "ip4", 
                    8080, false, true);
    server.AddSocket(ssocket);
  
    TLSSocket tls;
    
    int clientSocket = 0;
    do {

        clientSocket = server.WaitClients();
    } while (!tls.Accept(clientSocket, context));

    std::cout << "READ";
    Buffer packet;
    int result = tls.ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        assert(!"Error Read packet");
        return;
     }


    int result1 = tls.WritePacket(packet);
    if (result1<=0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        assert (!"Erro Write packet");
        return;
    }

}

void TestTLSSndRcv()
{
    std::thread thr (StartTLSServer);

    sleep(5);
    
    TLSContext context(false); 

    int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    ClientSocket clientSocket(s); 

    TLSSocket tls;

    if (clientSocket.Connect("127.0.0.1", 8080)==false)
    {
        std::cout << "Error connection" << std::endl;
        return;
    }
    if (!tls.Connect(s, "127.0.0.1", context))
    {
        std::cout << "Error tls connection" << std::endl;
        return;
    }

    Buffer packetGet = HTTPRequest::CreateGET("127.0.0.1", "8080");
    int result1 = tls.WritePacket(packetGet);
    if (result1<=0)
    {
        perror ("Writepacket ");
        std::cout << "error packet write " << result1 << std::endl;
        assert (!"Erro Write packet");
        return;
    }


    Buffer packet;
    int result = tls.ReadPacket(packet);
    if (result<=0)
    {
        perror ("Readpacket ");
        std::cout << "error packet read " << result << std::endl;
        assert(!"Error Read packet");
        return;
     }

    thr.join();
    assert (packet==packetGet); 

}

void TestTLSRcv()
{ 
 
    //int s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //ClientSocket cSocket(s);

    //cSocket.Connect ("127.0.0.1", 2000);
    //cSocket.SetTimeout();
 //   cSocket.


 
    /*TLSSocket tls;

    tls.setSocket();
    tls.accept();

    tls.read();*/
}
