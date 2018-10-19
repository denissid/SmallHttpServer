#pragma once

#include <iostream>
#include <unistd.h>

void MakeDaemon()
{
	using namespace std;

	int r = daemon(1,1);
	if (r==1)
	{
		cout << "Error daemonise" << endl;
		return;
	}

	cout << "Launch daemon" << endl;
}



