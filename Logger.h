#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <format>

#define OUTPUT_TO_SCREEN 1

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

Logger& LogPrv();
Logger& LogErrorPrv();
void WriteLog(const std::string& message, bool isTimePrefix=true);
void WriteLog(const std::vector<std::string>& messages);

template <class T>
static Logger& operator<< (Logger &logger, const T& message)
{
    logger() << message;

#ifdef OUTPUT_TO_SCREEN 
    TimePrefix tm;
    std::cout << tm.GetString() <<"'"<< message<<"'";
#endif
    return logger;
}


static Logger& operator<< (Logger &logger, std::ostream&(*pManip)(std::ostream&))
{
    (*pManip)(logger());
    (*pManip)(std::cout);

    return logger;
}

static std::string getFormatString(const uint32_t size)
{ 
    std::string formatString; 
    for (int i=0; i<size; ++i)
    {
        formatString += "{}";
    }
    return formatString;
}

template <class... T>
static void log (const T... message)
{
    const uint32_t size = sizeof...(T);
    std::string formatString = getFormatString(size);

    std::string result = std::vformat(formatString.c_str(), std::make_format_args(message...));
    LogPrv() << result <<std::endl;
}

template <class... T>
static void logError (const T... message)
{
    const uint32_t size = sizeof...(T);
    std::string formatString = getFormatString(size);

    std::string result = std::vformat(formatString.c_str(), std::make_format_args(message...));
    LogErrorPrv() << result <<std::endl;
}



