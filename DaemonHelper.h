#pragma once

#include <iostream>
#include <unistd.h>

void MakeDaemon()
{
	using namespace std;
	int id = fork();
	switch (id)
	{
		case 0:
		{
			cout << "child process started daemon was created" << endl;
			break;
		}
		case -1:
		{
			cerr << "Error create daemon" << endl;
			break;
		}
		default:
			cout << "parent killed" << endl;
			exit(0);
			break;
	}
}



