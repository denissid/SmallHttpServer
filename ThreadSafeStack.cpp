#include "ThreadSafeStack.h"
#include "Exceptions.h"

#include <chrono>

using namespace std;
using namespace std::chrono_literals;

void ThreadSafeStack::AddSocket(int i)
{
	lock_guard<mutex> lock(m_mutex);
	
	m_stack.push(i);
	cv.notify_one();
}

int ThreadSafeStack::GetSocket() const
{
	unique_lock<mutex> lock (m_mutex);
	cv.wait_for(lock, 5s,[this]{return !m_stack.empty();} );

    if (m_stack.empty())
        return -1;

	int s = m_stack.top();
	m_stack.pop();
	return s;
}

ThreadSafeStack::~ThreadSafeStack()
{
    //there can be deadlock 
    //in case of cv waiting 
}

