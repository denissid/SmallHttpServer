#include <iostream>
#include <stdlib.h>

#include <thread>

#include "Server.h"

#include "Options.h"
#include "DaemonHelper.h"
#include "Tests.h"
#include "Worker.h"

#include <vector>



int main (int argc, char** argv)
{
	//TestGETParse();
	using namespace std;

	Options options(argc, argv);

	cout << options.GetIP() << endl;
	cout << options.GetPort() << endl;
	cout << options.GetDirectory() << endl;
	
	MakeDaemon();

	Server server (options.GetIP(), options.GetPort());

	int s = server.WaitClients();
{
	//thread thr;
	//thr.join();
	Worker (s, options.GetDirectory());
}

	return 0;
}
