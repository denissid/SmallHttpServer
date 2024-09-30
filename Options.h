#pragma once 

#include <string>
#include <vector>

namespace ipFamily
{
    const std::string ip4 = "ip4";
    const std::string ip6 = "ip6";
};

struct Option
{
    std::string m_familyAddress; //ipv6 ipv4
    std::string m_ipAddress;
    int m_port;
    std::string m_directory;
    bool m_isSecure = false;

 
    Option (const std::string &family, 
            const std::string &address, 
            int port, 
            const std::string &directory, 
            bool isSecure = false): m_familyAddress(family), 
                            m_ipAddress(address),
                            m_port(port),
                            m_directory(directory),
                            m_isSecure(isSecure)
    {
    }
    
    static bool secureToBool(const std::string &secure)
    {
        return secure=="true" ? true : false;
    }
};

class Options
{
        std::vector<Option> m_options;

	public:

		Options(int argc, char** argv);
		~Options();

        std::string GetIPFamily() const;
		std::string GetIP() const;
		int GetPort() const;
		std::string GetDirectory() const;

	private:
		
		void LoadConfig(const Option& option);
};
