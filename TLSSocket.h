#pragma once

#include <iostream>

#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "HTTPPacket.h"
#include "SocketHelper.h"

class TLSContext 
{
        SSL_CTX *m_ctx = nullptr;

    public:
        TLSContext(bool server)
        { 
            SSL_library_init();
            OpenSSL_add_all_algorithms();
            SSL_load_error_strings();

            m_ctx = SSL_CTX_new(server?TLS_server_method():TLS_client_method());
            if (!m_ctx)
            {
                std::cout << "Error ctx" << std::endl;
                return;
            }

            if (!SSL_CTX_use_certificate_file(m_ctx, "certificates/cert.pem", SSL_FILETYPE_PEM) ||
                !SSL_CTX_use_PrivateKey_file(m_ctx, "certificates/key.pem", SSL_FILETYPE_PEM) )
            {
                SSL_CTX_free(m_ctx);
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

        bool Connect(int socket, const std::string &hostname, const TLSContext &context)
        { 
            CreateSSLConnection(socket, context);
            if (!SSL_set_tlsext_host_name(m_ssl, hostname.data()))
            {
                ERR_print_errors_fp(stderr);
                std::cout << "Error hostname " << std::endl;
                return false;
            }

            if (SSL_connect(m_ssl)==-1)
            {
                ERR_print_errors_fp(stderr);
                std::cout << "Error ssl Connect " << std::endl;
                return false;
            }

            std::cout << "Using " << SSL_get_cipher(m_ssl) << std::endl;

            X509 *cert = SSL_get_peer_certificate(m_ssl);
            if (!cert)
            {
                std::cout << "ssl peer certificate failed" << std::endl;
                return false;
            }

            char* tmp = nullptr;
            if ((tmp=X509_NAME_oneline(X509_get_subject_name(cert), 0, 0)))
            {
                std::cout << "subj" <<tmp ;
                OPENSSL_free(tmp);
            }

            if ((tmp=X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0)))
            {
                std::cout << "issuer" << tmp;
                OPENSSL_free(tmp);
            }

            X509_free(cert);

            return true;
        }

        bool Accept(int socket, const TLSContext &context) 
        { 
            CreateSSLConnection(socket, context);

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

        int ReadPacket (Buffer& buffer) const
        {
            return Read (buffer, m_ssl, 0);
        }

        int WritePacket(const Buffer& buffer) const
        {
            return Write(buffer, m_ssl, 0);
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

    private:
        void CreateSSLConnection(int socket, const TLSContext &context)
        { 
            m_ssl = SSL_new(context.Get());
            if(!m_ssl)
            {
                std::cout << "Error SSL_new" << std::endl;
            }

            SSL_set_fd(m_ssl, socket);
        }


};
