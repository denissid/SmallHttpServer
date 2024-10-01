#include "TLSSocket.h"
#include "ServerSocket.h"

void TestTLSCreating()
{
    TLSSocket tls;
}

void TestTLSSnd()
{
    TLSSocket tls;
    
    ServerSocket s("127.0.0.1", "ip4", 8080, true); 

    int clientSocket = 0;
    do {

        clientSocket = s.Accept();
        std::cout << clientSocket << std::endl;
    } while (!tls.accept(clientSocket));

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
