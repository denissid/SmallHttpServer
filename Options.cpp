#include "Options.h"

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>
#include <nlohmann/json.hpp>

#include "Logger.h"

Options::Options(int argc, char** argv): 
    m_familyAddress(ipFamily::ip4), //ip6
    m_ipAddress("127.0.0.1"),
    m_port(2000),
    m_directory("/home/denis/Stepick/Final")
{
	using namespace std;
	
	LoadConfig();
	
	Log() << "Options" << endl;
	int option = 0;
	do {
		option = getopt(argc, argv, "h:p:d:");
		switch (option)
		{
			case 'h':
			{
				char* address = optarg;
				m_ipAddress = address;
				break;
			}
			case 'p':
			{
				char* port = optarg;
				m_port=atoi(port);
				break;
			}
			case 'd':
			{
				char* directory = optarg;
				m_directory = directory;
				break;
			}
			case '?':
			{
				Log() << "Error";
				break;
			}
		}
	}
	while(option!=-1);

}

std::string Options::GetIP () const
{
	return m_ipAddress;
}

int Options::GetPort() const
{
	return m_port;
}

std::string Options::GetDirectory() const
{
	return m_directory;
}

std::string Options::GetIPFamily() const
{
    return m_familyAddress;
}

void Options::LoadConfig()
{
	using namespace std;
	using json = nlohmann::json;
	ifstream file("config.json");
	
	if (file.is_open())
	{
		Log() << "Load config" << endl;
		json j;
		file >> j;

        m_familyAddress = j["ipfamily"];
		m_ipAddress = j["address"];
		m_port = j["port"];
		m_directory = j["directory"];
	}
	else
	{
		Log() << "config.json wasn't found";
	}
    Log() << " address family " << m_familyAddress << std::endl;
    Log() << " address " << m_ipAddress << std::endl;
	Log() << " port " << m_port << std::endl;
	Log() << " directory:" << m_directory << std::endl;

}

Options::~Options()
{
}
