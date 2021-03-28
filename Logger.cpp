#include "Logger.h"

#include <iostream>
#include <chrono> 
#include <iomanip>
#include <sstream>

static Logger logger;

TimePrefix::TimePrefix()
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

std::string TimePrefix::GetStringTime() const
{
    return m_time;
}

size_t TimePrefix::GetSize() const
{
    return m_time.size();
}

Logger::Logger(): m_file("log.txt", std::fstream::in | std::fstream::out | std::fstream::app)
{
    if (!m_file.is_open())
    {
        throw std::runtime_error("Error open file");
    }
}

std::fstream& Log()
{
	TimePrefix timePrefix;
	logger() << timePrefix.GetStringTime() << " ";
	return logger();
}

#define OUTPUT_TO_SCREEN 1

void WriteLog(const std::string& message, bool isTimePrefix)
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

void WriteLog(const std::vector<std::string>& messages)
{
	bool isTimePrefix = true;
	for (auto a: messages)
	{
		WriteLog(a, isTimePrefix);
		isTimePrefix = false;
	}
}


