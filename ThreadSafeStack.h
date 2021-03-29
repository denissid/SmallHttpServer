#pragma once 

#include <stack>
#include <mutex>
#include <condition_variable>

class ThreadSafeStack
{
		mutable std::stack <int> m_stack;
		mutable std::mutex m_mutex;
		mutable std::condition_variable cv;

	public:
	
		ThreadSafeStack() = default;

		void AddSocket(int i);
		int GetSocket() const;

		~ThreadSafeStack();

	private:
        
		ThreadSafeStack(const ThreadSafeStack&);
		ThreadSafeStack& operator= (const ThreadSafeStack&);
};
