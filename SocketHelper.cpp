#include "SocketHelper.h"

std::unique_ptr<Socket>  Acceptor::operator() () const
{ 
    sockaddr_storage clientAddress = {0};
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept (m_socket, (sockaddr*)&clientAddress, &clientLen);
    if (clientSocket<0)
    {
        LogError() << errno << std::endl;
        return nullptr;
    }
    char address_buffer[100]={0};
    getnameinfo((sockaddr*) &clientAddress, clientLen, address_buffer, sizeof(address_buffer),
            0, 0, NI_NUMERICHOST);

    std::string s(address_buffer);
    Log() << s << std::endl;

    if (!m_context)
        return std::make_unique<ClientSocket> (clientSocket);

    TLSSocket tls(clientSocket, m_context);
    auto tlsSocket = std::unique_ptr<TLSSocket>(new TLSSocket(clientSocket, m_context));
      if (tlsSocket->Accept())
        return tlsSocket;
    return nullptr;
}

