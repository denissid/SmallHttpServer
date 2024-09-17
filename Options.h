#pragma once 

#include <string>

namespace ipFamily
{
    const std::string ip4 = "ip4";
    const std::string ip6 = "ip6";
};

class Options
{
        std::string m_familyAddress; //ipv6 ipv4
		std::string m_ipAddress;
		int m_port;
		std::string m_directory;

	public:

		Options(int argc, char** argv);
		~Options();

		std::string GetIP() const;
		int GetPort() const;
		std::string GetDirectory() const;
        std::string GetIPFamily() const;

	private:
		
		void LoadConfig();
};
