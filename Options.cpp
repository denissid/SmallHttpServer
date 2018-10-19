#include "Options.h"

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>

#include <fstream>
#include <nlohmann/json.hpp>

Options::Options(int argc, char** argv): m_ipAddress("127.0.0.1"),m_port(2000),m_directory("/home/denis/Stepick/Final")
{
	using namespace std;
	

	LoadConfig();
	
	cout << "Options" << endl;
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
				cout << "Error" << endl;
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

void Options::LoadConfig()
{
	using namespace std;
	using json = nlohmann::json;
	ifstream file("config.json");
	
	if (file.is_open())
	{
		cout << endl << "Load config" << endl;
		json j;
		file >> j;

		m_ipAddress = j["address"];
		m_port = j["port"];
		m_directory = j["directory"];
	}
	else
	{
		cout << "config.json wasn't found" << endl;
	}
}

Options::~Options()
{
}
