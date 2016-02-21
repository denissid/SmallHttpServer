#include "Options.h"

#include <iostream>

int main (int argc, char** argv)
{
	using namespace std;

	Options options(argc, argv);

	cout << options.GetIP()<<endl;
	cout << options.GetPort()<<endl;
	cout << options.GetDirectory()<<endl;

	return 0;
}
