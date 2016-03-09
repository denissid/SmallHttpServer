#include "ThreadSafeStack.h"

using namespace std;

void ThreadSafeStack::AddSocket(int i)
{
	lock_guard<mutex> lock(m_mutex);
	
	m_stack.push(i);
	
	cv.notify_one();
}

int ThreadSafeStack::GetSocket() const
{
	unique_lock<mutex> lock (m_mutex);
	cv.wait(lock, [this]{return !m_stack.empty();} );

	int s = m_stack.top();
	m_stack.pop();
	return s;
}
