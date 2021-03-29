#include "Logger.h"

#include <iostream>
#include <chrono> 
#include <iomanip>
#include <sstream>

static Logger logger;

using namespace std;
using namespace std::chrono;

TimePrefix::TimePrefix()
{

    auto now = system_clock::now();
    auto microsec = duration_cast<microseconds> (now.time_since_epoch()%seconds(1));

    auto time_t = system_clock::to_time_t(now);
    auto local_time = localtime(&time_t);
    auto time = put_time(local_time, "%F %T");

    stringstream tmp;
    tmp << time;	
    m_time = tmp.str() + string(":") + to_string(microsec.count());
}

string TimePrefix::GetString() const
{
    return m_time;
}

size_t TimePrefix::GetSize() const
{
    return m_time.size();
}

Logger::Logger(): m_file("log.txt", fstream::in | fstream::out | fstream::app)
{
    if (!m_file.is_open())
    {
        throw runtime_error("Error open file");
    }
}

fstream& Log()
{
	TimePrefix timePrefix;
	logger() << timePrefix.GetString() << " ";
	return logger();
}

#define OUTPUT_TO_SCREEN 1

void WriteLog(const string& message, bool isTimePrefix)
{
	using namespace std;

	TimePrefix timePrefix;
	if (isTimePrefix)
	{
		logger() << timePrefix.GetString() <<" '"<< message << "' "<< endl;

#ifdef OUTPUT_TO_SCREEN 
        cout << timePrefix.GetString() <<" '"<< message << "' "<< endl;
#endif

	}
	else
	{
		logger() << setw(timePrefix.GetSize()) <<" '"<< message << "' "<< endl;

#ifdef OUTPUT_TO_SCREEN 
        cout << setw(timePrefix.GetSize()) <<" '"<< message << "' "<< endl;
#endif

	}
}

void WriteLog(const vector<string>& messages)
{
	bool isTimePrefix = true;
	for (auto a: messages)
	{
		WriteLog(a, isTimePrefix);
		isTimePrefix = false;
	}
}


