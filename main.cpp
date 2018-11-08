#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>
#include <functional>

#include "Server.h"
#include "Options.h"
#include "DaemonHelper.h"
#include "Tests.h"
#include "Worker.h"
#include "ThreadSafeStack.h"
#include "Logger.h"


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
		
		ThreadSafeStack stack;
		CreateThreads (stack, options.GetDirectory());
		
		Server server (options.GetIP(), options.GetPort());

		do
		{
			int s = server.WaitClients();
			stack.AddSocket(s);
		}
		while(1);
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
