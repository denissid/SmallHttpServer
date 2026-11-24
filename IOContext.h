#pragma once
#include "Socket.h"
#include <memory>
#include <cassert>

struct IOContext
{
    enum class Type
    {
        Read,
        Write,
        None
    } m_tp;

    Buffer m_buffer;
    ssize_t m_size = -1;
    std::shared_ptr<Socket> m_Socket;

    IOContext(IOContext::Type tp, std::unique_ptr<Socket> socket): m_size(0), m_tp(tp)
    {
        m_Socket = std::move(socket);
    }

    IOContext(IOContext &&t)
    {
        m_tp = t.m_tp;
        m_buffer = std::move(t.m_buffer);
        m_size = t.m_size;
        m_Socket = std::move(t.m_Socket);

        t.m_tp = Type::None;
        t.m_buffer.clear();
        t.m_size = -1;
    }

    IOContext& operator= (IOContext &&r) noexcept
    {
        m_tp = r.m_tp;
        m_buffer = std::move(r.m_buffer);
        m_size = r.m_size;
        m_Socket = std::move(r.m_Socket);

        r.m_tp = Type::None;
        r.m_buffer.clear();
        r.m_size = -1;

        return *this;
    }

    IOContext(IOContext &) = delete;
    IOContext& operator= (IOContext &) = delete;

    void SetSocket(std::unique_ptr<Socket> s)
    {
        m_Socket = std::move(s);
    }

    auto GetSocket() const
    {
        if (m_Socket.get()==nullptr)
        {
            assert(!"STOP GetSocjet");
        }
        return m_Socket;
    }

    Buffer& GetRefBuffer()
    {
        return m_buffer;
    }

    void SetSize(ssize_t s)
    {
        m_size = s;
    }

    ssize_t GetSize() const
    {
        return m_size;
    }

    Type GetType() const
    {
        return m_tp;
    }

    void SetType(IOContext::Type tp) 
    {
        m_tp = tp;
    }

};


