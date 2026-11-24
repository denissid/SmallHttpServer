#include <iostream>

#include "Tests/Tests.h"
#include "Tests/TLSTests.h"
#include "Tests/SndRcvTest.h"
#include "Tests/SndRcvUnbTest.h"

int main (int argc, char** argv)
{
    TestExtract();
	TestGETParse();
	TestSplit();
    TestGetContentType();

    TestSndRcv();

    TestConnect();

    TestTLSCreating();
    TestTLSSndRcv();

    TestEAGAINWrite();
    TestEAGAINRead();
	return 0;
}
