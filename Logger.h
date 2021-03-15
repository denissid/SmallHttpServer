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


class Logger 
{
	public:
		Logger(): m_file("log.txt", std::fstream::in | std::fstream::out | std::fstream::app)
		{
			if (!m_file.is_open())
			{
				throw std::runtime_error("Error open file");
			}
		}

		std::fstream& operator() ()
		{
			return m_file;
		}

	private:

		std::fstream m_file;
};

static Logger logger;

static std::fstream& Log()
{
	TimePrefix timePrefix;
	logger() << timePrefix.GetStringTime() << " ";
	return logger();
}

#define OUTPUT_TO_SCREEN 1

static void WriteLog(const std::string& message, bool isTimePrefix=true)
{
	using namespace std;

	TimePrefix timePrefix;
	if (isTimePrefix)
	{
		logger() << timePrefix.GetStringTime() <<" '"<< message << "' "<< endl;
#ifdef OUTPUT_TO_SCREEN 
        std::cout << timePrefix.GetStringTime() <<" '"<< message << "' "<< endl;
#endif

	}
	else
	{
		logger() << setw(timePrefix.GetSize()) <<" '"<< message << "' "<< endl;

#ifdef OUTPUT_TO_SCREEN 
        std::cout << setw(timePrefix.GetSize()) <<" '"<< message << "' "<< endl;
#endif

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


