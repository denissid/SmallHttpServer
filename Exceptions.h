#pragma once
#include <exception>
#include <string>

struct NetException: public std::exception
{
	std::string m_reason;
	
	NetException(const std::string& reason): m_reason(reason)
	{
	}
	virtual const char* what() const throw()
	{
		return m_reason.c_str();
	}
};

struct ParseException: public std::exception
{
	std::string m_reason;

	ParseException(const std::string& reason):m_reason(reason)
	{
	}

	virtual const char* what() const throw()
	{
		return m_reason.c_str();
	}	
};
