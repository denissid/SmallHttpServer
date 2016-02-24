#include <iostream>
#include <stdlib.h>

#include <thread>

#include "Server.h"

#include "Options.h"
#include "DaemonHelper.h"
#include "Tests.h"
#include "Worker.h"
#include "ThreadSafeStack.h"

#include <vector>

void CreateThreads (const ThreadSafeStack& stack, const std::string& folder)
{
	int i = 4;
	while(i--)
	{
		std::thread thr(std::bind(Worker, std::ref(stack), folder));
		thr.detach();
	}
}

int main (int argc, char** argv)
{
	TestGETParse();
	using namespace std;

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
	}while(1);

	return 0;
}
