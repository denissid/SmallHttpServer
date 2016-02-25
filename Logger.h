#pragma once

#include <fstream>
#include <string>

static void WriteLog(const std::string& message)
{
	using namespace std;

	static fstream file ("log.txt", std::fstream::in | std::fstream::out|std::fstream::app);
	file << message << endl;
}
