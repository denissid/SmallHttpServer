#pragma once

#include <iostream>
#include <unistd.h>

#include "Logger.h"

void MakeDaemon()
{
	using namespace std;

	int r = daemon(1,1);
	if (r==1)
	{
		log("Error daemonise");
		return;
	}

	log("Launch daemon");
}



