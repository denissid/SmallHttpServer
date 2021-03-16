#pragma once

#include "ThreadSafeStack.h"
#include <string>

extern std::atomic<bool> keep_thread_runnig;

void Worker (const ThreadSafeStack& stack, const std::string& directory);


