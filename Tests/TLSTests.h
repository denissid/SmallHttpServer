#include "TLSSocket.h"
#include "ServerSocket.h"

void TestTLSCreating()
{
    TLSSocket tls;
}

void TestTLSSnd()
{
    
    ServerSocket s("127.0.0.1", "ip4", 8080, true); 

    TLSContext context; 

    TLSSocket tls;
    int clientSocket = 0;
    do {

        clientSocket = s.Accept();
    } while (!tls.accept(clientSocket, context));

    tls.read();
  //  tls.write();
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
