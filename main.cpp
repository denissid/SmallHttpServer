#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>
#include <functional>
#include <signal.h>
#include <atomic>

#include "Server.h"
#include "ServerSocket.h"
#include "Options.h"
#include "DaemonHelper.h"
#include "Worker.h"
#include "ThreadSafeStack.h"
#include "Logger.h"
#include "TLSSocket.h"

extern std::atomic<bool> keepThreadRunning;

static void sig_handler(int )
{
    keepThreadRunning = false;
}

void CreateThreads (const ThreadSafeStack& stack, const std::string& folder)
{
	using namespace std;
	int i = 20;//thread::hardware_concurrency();
	Log() << "Count of threads "+ to_string(i) <<endl;
	while(i--)
	{
		thread thr(std::bind(Worker, std::ref(stack), folder));
		thr.detach();
	}
}

int main (int argc, char** argv)
{
	using namespace std;

    signal(SIGINT, sig_handler);

	try
	{
		Options options(argc, argv);

		Log() << options.GetIPFamily() << endl;
		Log() << options.GetIP() << endl;
		Log() << options.GetPort() << endl;
		Log() << options.GetDirectory() << endl;
		
	//	MakeDaemon();
		Server server;
        ServerSocket ssocket(options.GetIP(), options.GetIPFamily(), options.GetPort(), false);

        server.AddSocket(ssocket);

		ThreadSafeStack stack;
		CreateThreads (stack, options.GetDirectory());

        this_thread::sleep_for(1s);
		do
		{
			int s = server.WaitClients();
			stack.AddSocket(s);
		}
		while(keepThreadRunning);
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
