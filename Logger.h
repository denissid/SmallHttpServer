#pragma once

#include <vector>
#include <fstream>
#include <string>

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

std::fstream& Log();
void WriteLog(const std::string& message, bool isTimePrefix=true);
void WriteLog(const std::vector<std::string>& messages);

