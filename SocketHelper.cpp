#include "SocketHelper.h"

std::unique_ptr<Socket>  Acceptor::operator() () const
{ 
    int clientSocket = AcceptSocket();

    if (clientSocket<0)
        return nullptr;

    if (m_tlsContext==nullptr)
    {
        return std::make_unique<ClientSocket> (clientSocket);
    }

    TLSSocket tls(clientSocket, m_tlsContext);
    auto tlsSocket = std::unique_ptr<TLSSocket>(new TLSSocket(clientSocket, m_tlsContext));
      if (tlsSocket->Accept())
        return tlsSocket;
    return nullptr;
}

int Acceptor::AcceptSocket() const
{ 
    sockaddr_storage clientAddress = {0};
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept (m_socket, (sockaddr*)&clientAddress, &clientLen);
    if (clientSocket<0)
    {
        logError("accept ", errno);
        return -1;
    }
    char address_buffer[100]={0};
    getnameinfo((sockaddr*) &clientAddress, clientLen, address_buffer, sizeof(address_buffer),
            0, 0, NI_NUMERICHOST);

    std::string s(address_buffer);
    log(s);
    
    return clientSocket;
}

int SetNonblock(int fd)
{
    int flags = 0;
#if defined(O_NONBLOCK)
    if (-1==(flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl (fd, F_SETFL, flags| O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

int SetBlock(int fd)
{
    int flags = 0;
#if defined(O_NONBLOCK)
    if (-1==(flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    flags &= ~O_NONBLOCK;
    log( "flags ", flags);
    return fcntl (fd, F_SETFL, flags);
#else
    return ioctl(fd, FIOBIO, &flags);
#endif
}


