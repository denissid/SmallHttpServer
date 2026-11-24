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

        std::tuple<int, ssize_t> ReadPacket (Buffer& buffer, size_t size) const;
        std::tuple<int, ssize_t> WritePacket(const Buffer& buffer, size_t size) const;

        int Get() const
        {
            return m_socket;
        }

        void shutdown();

        TLSSocket(const TLSSocket&) = delete;
        TLSSocket &operator= (const TLSSocket &) = delete;

    private:

        void CreateSSL(int socket, TLSContext *context);
        
};

