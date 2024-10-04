#include "TLSContext.h"

TLSContext::TLSContext(bool isServer)
{ 
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    m_ctx = SSL_CTX_new(isServer?TLS_server_method():TLS_client_method());
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

TLSContext::~TLSContext()
{
    SSL_CTX_free(m_ctx);
    m_ctx = nullptr;
} 

SSL_CTX *TLSContext::Get() const
{
    return m_ctx;
}



