#pragma once

#include <iostream>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "HTTPPacket.h"
#include "SocketHelper.h"
#include "TLSContext.h"

class TLSSocket
{
        SSL *m_ssl = nullptr;

    public:

        TLSSocket();
        ~TLSSocket();

        bool Connect(int socket, const std::string &hostname, const TLSContext &context);
        bool Accept(int socket, const TLSContext &context);
        int ReadPacket (Buffer& buffer) const;
        int WritePacket(const Buffer& buffer) const;

        void shutdown();

    private:

        void CreateSSLConnection(int socket, const TLSContext &context);
        
};
