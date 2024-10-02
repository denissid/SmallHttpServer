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
        TLSContext()
        { 
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();

            m_ctx = SSL_CTX_new(TLS_server_method());
            if (!m_ctx)
            {
                std::cout << "Error ctx" << std::endl;
                return;
            }

            if (!SSL_CTX_use_certificate_file(m_ctx, "certificates/cert.pem", SSL_FILETYPE_PEM) ||
                !SSL_CTX_use_PrivateKey_file(m_ctx, "certificates/key.pem", SSL_FILETYPE_PEM) )
            {
                std::cout << "Error use cerificate file" << std::endl;
                return;
            }
        }

        ~TLSContext()
        {
            SSL_CTX_free(m_ctx);
            m_ctx = nullptr;
        }

        TLSContext (const TLSContext &) = delete;
        TLSContext& operator= (const TLSContext&) = delete;

        friend class TLSSocket;

    private: 
        SSL_CTX *Get() const
        {
            return m_ctx;
        }
};

class TLSSocket
{
        SSL *m_ssl = nullptr;

    public:

        TLSSocket()
        {
        }

        bool accept(int socket, const TLSContext &context) 
        { 
            m_ssl = SSL_new(context.Get());
            if(!m_ssl)
            {
                std::cout << "Error SSL_new" << std::endl;
            }


            SSL_set_fd(m_ssl, socket);

            if (SSL_accept(m_ssl)<=0)
            {
                std::cout << "Error SSL accept" << std::endl;
                ERR_print_errors_fp(stderr);
                shutdown();
                SSL_free(m_ssl);
                m_ssl = nullptr;

                return false;
            }

            std::cout << "Using " << SSL_get_cipher(m_ssl) << std::endl;
            return true;
        }

        void read ()
        {
            char request[1024] = {0};
            SSL_read(m_ssl, request, 1024);
            std::cout << "'" << request << "'" ;
        }

        void write()
        {
            char data[1024] = {0};
            SSL_write(m_ssl, data, 1024);
        }

        void shutdown()
        {
            if (m_ssl!=nullptr)
                SSL_shutdown(m_ssl);
        }

        ~TLSSocket()
        {
            if (m_ssl!=nullptr)
                SSL_free(m_ssl);

        }
};
