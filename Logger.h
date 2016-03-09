#pragma once

#include <iostream>
#include <fstream>
#include <string>

static void WriteLog(const std::string& message)
{
	using namespace std;

	static fstream file ("log.txt", std::fstream::in | std::fstream::out|std::fstream::app);
	if (file)
	{
		file << message << endl;
	}
	else
	{
		cout << "Error log" << endl;
	}
}
