#pragma once 

#include <iostream>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


class TLSContext 
{
        SSL_CTX *m_ctx = nullptr;

    public:

        TLSContext(bool server);
        ~TLSContext();

        TLSContext (const TLSContext &) = delete;
        TLSContext& operator= (const TLSContext&) = delete;

        friend class TLSSocket;

    private: 

        SSL_CTX *Get() const;
};


