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

int main (int argc, char** argv)
{

    TestExtract();
	TestGETParse();
	TestSplit();

	return 0;
}
