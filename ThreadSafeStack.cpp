#include "ThreadSafeStack.h"
#include "Exceptions.h"
#include "Logger.h"

#include <chrono>

using namespace std;
using namespace std::chrono_literals;

void ThreadSafeStack::AddContext(std::unique_ptr<IOContext> i)
{
    {
	    unique_lock lock (m_mutex);
	    m_stack.push(std::move(i));
    }
	cv.notify_one();
}

std::expected<std::unique_ptr<IOContext>, ThreadStackErr> ThreadSafeStack::GetContext(std::stop_token st) const
{ 
    if (st.stop_requested())
    {
        logError("Program was interrupted");
        return std::unexpected(ThreadStackErr::INTERRUPTED);
    }


	std::unique_lock lock(m_mutex);
	cv.wait_for(lock, st, 5s, [this, st]{return !m_stack.empty() || st.stop_requested();});

    if (st.stop_requested())
    {
        logError("Program was interrupted");
        return std::unexpected(ThreadStackErr::INTERRUPTED);
    }

    if (m_stack.empty())
    {
        return std::unexpected(ThreadStackErr::TIMEOUT);
    }

	auto s = std::move(m_stack.top());
	m_stack.pop();
	return s;
}

ThreadSafeStack::~ThreadSafeStack()
{
    //there can be deadlock 
    //in case of cv waiting 
}

