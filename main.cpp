#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>
#include <functional>
#include <signal.h>
#include <atomic>
#include <memory>
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

std::atomic_flag gStop(false);
static void sig_handler(int )
{
    gStop.test_and_set();
}


void CreateThreads (const ThreadSafeStack& stack, const std::string& folder)
{
	using namespace std;
	int i = 20;//thread::hardware_concurrency();
	log("Count of threads ", i);
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
            auto ssocket = std::make_unique<ServerSocket>  ( options.GetIP(i), options.GetIPFamily(i), 
                    options.GetPort(i), false, options.IsSecure(i)  );
            server.AddSocket(std::move(ssocket) );
        }

		ThreadSafeStack stack;
		CreateThreads (stack, options.GetDirectory(0));

        auto token = stopSource.get_token();
        int stopFd = server.GetStopFd();
        jthread controlThread([stopFd] () { 
                    while(!gStop.test())
                    {
                    }
                    log("stopped was sent");
                    stopSource.request_stop();
                    uint64_t val = 1;
                    write(stopFd, &val, sizeof(val)); 
                } );

        controlThread.detach();

        this_thread::sleep_for(1s);
		do
		{
			auto context = server.WaitClients(token);
			stack.AddContext(std::move(context));
        }
		while(!token.stop_requested());

        for (auto &&t: threads)
        {
            t.request_stop();
            t.join();
        }

	}
	catch(std::exception& e)
	{
		logError("Exception ", e.what());
	}
	catch (...)
	{
		logError("Unknown exception");
	}

	return 0;
}
