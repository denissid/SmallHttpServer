#include "Options.h"

#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <iostream>


Options::Options(int argc, char** argv): m_ipAddress("127.0.0.1"),m_port(2000),m_directory("/home/denis/Stepick/Final")
{
	using namespace std;

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

Options::~Options()
{
}
