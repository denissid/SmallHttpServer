#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>
#include <functional>
#include <signal.h>
#include <atomic>

#include "Server.h"
#include "Options.h"
#include "DaemonHelper.h"
#include "Tests.h"
#include "Worker.h"
#include "ThreadSafeStack.h"
#include "Logger.h"

extern std::atomic<bool> keep_thread_running;

static void sig_handler(int )
{
    keep_thread_running = false;
}

void CreateThreads (const ThreadSafeStack& stack, const std::string& folder)
{
	using namespace std;
	int i = thread::hardware_concurrency();
	Log() << "Count of threads " << i << endl;
	while(i--)
	{
		thread thr(std::bind(Worker, std::ref(stack), folder));
		thr.detach();
	}
}

int main (int argc, char** argv)
{
    signal(SIGINT, sig_handler);
	//TestGETParse();
	//TestSplit();

	using namespace std;

	try
	{
		Options options(argc, argv);

		Log() << options.GetIP() << endl;
		Log() << options.GetPort() << endl;
		Log() << options.GetDirectory() << endl;
		
	//	MakeDaemon();

		Server server (options.GetIP(), options.GetPort());

		ThreadSafeStack stack;
		CreateThreads (stack, options.GetDirectory());

        this_thread::sleep_for(1s);
		do
		{
			int s = server.WaitClients();
			stack.AddSocket(s);
		}
		while(keep_thread_running);
	}
	catch(std::exception& e)
	{
		Log() << e.what() << endl;
	}
	catch (...)
	{
		Log() << "Unknown exception" << endl;
	}

	return 0;
}
