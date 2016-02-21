#pragma once 

#include <string>

class Options
{
		std::string m_ipAddress;
		int m_port;
		std::string m_directory;

	public:

		Options(int argc, char** argv);
		~Options();

		std::string GetIP() const;
		int GetPort() const;
		std::string GetDirectory() const;
};
