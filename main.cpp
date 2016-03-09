#include <iostream>
#include <stdlib.h>

#include <thread>
#include <vector>
#include <exception>

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
	cout << "Count of threads " << i << endl;
	while(i--)
	{
		thread thr(std::bind(Worker, std::ref(stack), folder));
		thr.detach();
	}
}

int main (int argc, char** argv)
{
	//TestGETParse();
	using namespace std;

	try
	{
		Options options(argc, argv);

		cout << options.GetIP() << endl;
		cout << options.GetPort() << endl;
		cout << options.GetDirectory() << endl;
		
		MakeDaemon();
		
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
		cout << e.what() << endl;
	}
	catch (...)
	{
		cout << "Unknown exception" << endl;
	}

	return 0;
}
