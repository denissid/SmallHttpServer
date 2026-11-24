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

 std::unique_ptr<IOContext> ThreadSafeStack::GetContext(std::stop_token st) const
{
	std::unique_lock lock(m_mutex);
	cv.wait_for(lock, st, 5s, [this]{return !m_stack.empty();});

    if (st.stop_requested())
    {
        logError("Program was interupted");
        return nullptr;
    }

    if (m_stack.empty())
    {
        //logError("Stack empty");
        return nullptr;
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

