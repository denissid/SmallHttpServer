#pragma once 

#include "IOContext.h"

#include <stack>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <stop_token>


class ThreadSafeStack
{
		mutable std::stack <std::unique_ptr<IOContext> > m_stack;
		mutable std::mutex m_mutex;
		mutable std::condition_variable_any cv;

	public:
	
		ThreadSafeStack() = default;

		void AddContext(std::unique_ptr<IOContext> context);
		std::unique_ptr<IOContext>  GetContext(std::stop_token st) const;

		~ThreadSafeStack();

	private:
        
		ThreadSafeStack(const ThreadSafeStack&);
		ThreadSafeStack& operator= (const ThreadSafeStack&);
};
