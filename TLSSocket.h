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
#include "Socket.h"

class TLSSocket: public Socket
{
        SSL *m_ssl = nullptr;
        int m_socket = -1;
        TLSContext *m_context=nullptr;

    public:

        TLSSocket(int socket, TLSContext *context);
        ~TLSSocket();

        bool Connect(const std::string &hostname);
        bool Accept();
        void SetTimeout ();
        bool IsAlive() const;
        int ReadPacket (Buffer& buffer) const;
        int WritePacket(const Buffer& buffer) const;

        void shutdown();

        TLSSocket(const TLSSocket&) = delete;
        TLSSocket &operator= (const TLSSocket &) = delete;

    private:

        void CreateSSL(int socket, TLSContext *context);
        
};

