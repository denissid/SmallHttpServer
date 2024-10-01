#include "Options.h"

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>
#include <nlohmann/json.hpp>

#include "Logger.h"

Options::Options(int argc, char** argv)
{
	using namespace std;
	

	Log() << "Options" << endl;
	Option commandOption {ipFamily::ip4, 
                          std::string("127.0.0.1"), 
                          2000, 
                          std::string("/home/denis/Stepick/Final")};
	int option = 0;
	do {
		option = getopt(argc, argv, "h:p:d:");
		switch (option)
		{
			case 'h':
			{
				char* address = optarg;
                commandOption.m_ipAddress = address;
				break;
			}
			case 'p':
			{
				char* port = optarg;
                commandOption.m_port = atoi(port);
				break;
			}
			case 'd':
			{
				char* directory = optarg;
                commandOption.m_directory = directory;
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

	LoadConfig(commandOption);
}

std::string Options::GetIPFamily(int index) const
{
    return m_options[index].m_familyAddress;
}

std::string Options::GetIP (int index) const
{
	return m_options[index].m_ipAddress;
}

int Options::GetPort(int index) const
{
	return m_options[index].m_port;
}

std::string Options::GetDirectory(int index) const
{
	return m_options[index].m_directory;
}

bool Options::IsSecure(int index) const 
{
	return m_options[index].m_isSecure;
}

size_t Options::GetSize() const
{
    return m_options.size();
}

void Options::LoadConfig(const Option &defOption)
{
	using namespace std;
	using json = nlohmann::json;
	ifstream file("config.json");
	
	if (file.is_open())
	{
		Log() << "Load config" << endl;
		json j;
		file >> j;

        for (json::iterator i = j.begin(); i!=j.end(); ++i)
        {
            json s = *i;

            std::string familyAddress = s["ipfamily"];
            std::string ipAddress = s["address"];
		    int port = s["port"];
            std::string directory = s["directory"];

            Log() << " address family " + familyAddress << std::endl;
            Log() << " address " + ipAddress << std::endl;
	        Log() << " port " + to_string(port) << std::endl;
	        Log() << " directory: " + directory << std::endl;

            std::string secure = s["secure"];
            Log() << " secure " + secure << std::endl;


            Option option{familyAddress, ipAddress, port, directory, Option::secureToBool(secure)};
            m_options.push_back (option);
        }
    }
	else
	{
		Log() << "config.json wasn't found";
        
        m_options.push_back(defOption);
        Log() << " address family " + m_options.at(0).m_familyAddress << std::endl;
        Log() << " address " + m_options.at(0).m_ipAddress << std::endl;
	    Log() << " port " + std::to_string(m_options.at(0).m_port) << std::endl;
	    Log() << " directory: " + m_options.at(0).m_directory << std::endl;
	}
 }

Options::~Options()
{
}
