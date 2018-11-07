#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <chrono> 
#include <iomanip>
#include <sstream>
#include <vector>

class TimePrefix 
{

	std::string m_time;

	public:

	TimePrefix()
	{
		using namespace std;

		auto now = chrono::system_clock::now();
		auto microsec = chrono::duration_cast<chrono::microseconds> (now.time_since_epoch()%chrono::seconds(1));

		auto time_t = chrono::system_clock::to_time_t(now);
		auto local_time = std::localtime(&time_t);
		auto time = put_time(local_time, "%F %T");

		stringstream tmp;
		tmp << time;	
		m_time = tmp.str() + std::string(":") + std::to_string(microsec.count());
	}

	std::string GetStringTime() const
	{
		return m_time;
	}

	size_t GetSize() const
	{
		return m_time.size();
	}
};

static void WriteLog(const std::string& message, bool isTimePrefix=true)
{
	using namespace std;

	static fstream file ("log.txt", std::fstream::in | std::fstream::out | std::fstream::app);
	if (file)
	{
		TimePrefix timePrefix;
		if (isTimePrefix)
		{
			file << timePrefix.GetStringTime() <<" '"<< message << "' "<< endl;
		}
		else
		{
			file << setw(timePrefix.GetSize()) <<" '"<< message << "' "<< endl;
		}
	}
	else
	{
		cout << "Error open of log file" << endl;
	}
}

static void WriteLog(const std::vector<std::string>& messages)
{
	bool isTimePrefix = true;
	for (auto a: messages)
	{
		WriteLog(a, isTimePrefix);
		isTimePrefix = false;
	}
}


