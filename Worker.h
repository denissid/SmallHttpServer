#pragma once

#include <string>
#include <atomic>

class ThreadSafeStack;

extern std::atomic<bool> keep_thread_runnig;

void Worker (const ThreadSafeStack& stack, const std::string& directory);


