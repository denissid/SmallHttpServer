#include <iostream>

#include "Tests/Tests.h"
#include "Tests/TLSTests.h"
#include "Tests/SndRcvTest.h"

int main (int argc, char** argv)
{
    TestExtract();
	TestGETParse();
	TestSplit();
    TestGetContentType();

    TestSndRcv();

    //TestTLSCreating();
    //TestTLSSnd();

	return 0;
}
