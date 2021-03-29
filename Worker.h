#pragma once

#include <string>
#include <atomic>

class ThreadSafeStack;

void Worker (const ThreadSafeStack& stack, const std::string& directory);


