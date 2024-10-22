#pragma once

#include <string>
#include <atomic>
#include <stop_token>

class ThreadSafeStack;

void Worker (std::stop_token st, const ThreadSafeStack& stack, const std::string& directory);


