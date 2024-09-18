#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

class TimePrefix 
{
	    std::string m_time;

	public:

        TimePrefix();
        std::string GetString() const;        
        size_t GetSize() const;

        TimePrefix(const TimePrefix&) = default;
        TimePrefix& operator= (const TimePrefix&) = default;
};

class Logger 
{
		std::fstream m_file;

	public:
        
        Logger ();

		std::fstream& operator() ()
		{
			return m_file;
		}

};

Logger& Log();
Logger& LogError();
void WriteLog(const std::string& message, bool isTimePrefix=true);
void WriteLog(const std::vector<std::string>& messages);

template <class T>
static Logger& operator<< (Logger &log, const T& message)
{
    log() << message;
    TimePrefix tm;
    std::cout << tm.GetString() <<"'"<< message<<"'";
    return log;
}


static Logger& operator<< (Logger &log, std::ostream&(*pManip)(std::ostream&))
{
    (*pManip)(log());
    (*pManip)(std::cout);

    return log;
}



