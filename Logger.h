#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono> 
#include <iomanip>

static void WriteLog(const std::string& message)
{
	using namespace std;

	static fstream file ("log.txt", std::fstream::in | std::fstream::out|std::fstream::app);
	if (file)
	{
		auto now = chrono::system_clock::now();
		auto m = chrono::duration_cast<chrono::microseconds> (now.time_since_epoch()%chrono::seconds(1));

		auto time_t = chrono::system_clock::to_time_t(now);
		auto local_time = std::localtime(&time_t);
		auto time = put_time(local_time, "%F %T");
		cout << time << " " <<   
		m.count()
		<<" '"<< message << "' "<< endl;
	}
	else
	{
		cout << "Error log" << endl;
	}
}
