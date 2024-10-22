#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>
#include <functional>
#include <signal.h>
#include <atomic>
#include <stop_token>

#include "Server.h"
#include "ServerSocket.h"
#include "Options.h"
#include "DaemonHelper.h"
#include "Worker.h"
#include "ThreadSafeStack.h"
#include "Logger.h"
#include "TLSSocket.h"

std::vector<std::jthread> threads;
std::stop_source stopSource;

static void sig_handler(int )
{
    stopSource.request_stop();
}


void CreateThreads (const ThreadSafeStack& stack, const std::string& folder)
{
	using namespace std;
	int i = 20;//thread::hardware_concurrency();
	Log() << "Count of threads "+ to_string(i) <<endl;
	while(i--)
	{
		jthread thr(Worker, stopSource.get_token(), std::ref(stack), folder);
        threads.push_back(move(thr));
	}
}

int main (int argc, char** argv)
{
	using namespace std;

    signal(SIGINT, sig_handler);

	try
	{
		Options options(argc, argv);

	//	MakeDaemon();
		size_t size = options.GetSize();
		Server server;
        for (int i=0; i<size; ++i)
        {
            ServerSocket ssocket(options.GetIP(i), options.GetIPFamily(i), 
                    options.GetPort(i), false, options.IsSecure(i));
            server.AddSocket(ssocket);
        }

		ThreadSafeStack stack;
		CreateThreads (stack, options.GetDirectory(0));

        this_thread::sleep_for(1s);
        auto t = stopSource.get_token();
		do
		{
			auto s = server.WaitClients(t);
			stack.AddSocket(std::move(s));
		}
		while(!t.stop_requested());

        for (auto &&t: threads)
        {
            t.join();
        }

	}
	catch(std::exception& e)
	{
		LogError() << "Exception "<< e.what() << endl;
	}
	catch (...)
	{
		LogError() << "Unknown exception" << endl;
	}

	return 0;
}
