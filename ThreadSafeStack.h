#pragma once 

#include "Socket.h"

#include <stack>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <stop_token>


class ThreadSafeStack
{
		mutable std::stack <std::unique_ptr<Socket> > m_stack;
		mutable std::mutex m_mutex;
		mutable std::condition_variable_any cv;

	public:
	
		ThreadSafeStack() = default;

		void AddSocket(std::unique_ptr<Socket> socket);
		std::unique_ptr<Socket>  GetSocket(std::stop_token st) const;

		~ThreadSafeStack();

	private:
        
		ThreadSafeStack(const ThreadSafeStack&);
		ThreadSafeStack& operator= (const ThreadSafeStack&);
};
