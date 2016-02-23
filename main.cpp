#include <iostream>
#include <unistd.h>
#include <stdlib.h>

#include "Options.h"
#include "Server.h"
#include "ClientSocket.h"

#include <vector>
void MakeDaemon()
{
	using namespace std;
	int id = fork();
	switch (id)
	{
		case 0:
		{
			cout << "child process started daemon was created" << endl;
			break;
		}
		case -1:
		{
			cerr << "Error create daemon" << endl;
			break;
		}
		default:
			cout << "parent killed" << endl;
			exit(0);
			break;
	}
}

int main (int argc, char** argv)
{
	using namespace std;

	Options options(argc, argv);

	cout << options.GetIP() << endl;
	cout << options.GetPort() << endl;
	cout << options.GetDirectory() << endl;
	
	//MakeDaemon();

	Server server (options.GetIP(), options.GetPort());

	int i = server.WaitClients();
	ClientSocket cs(i);
	Buffer buffer;

	cs.ReadPacket(buffer);
	cout << (char*)&buffer[0];

	static std::string  not_found = "HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n";
	static std::string  templ = "HTTP/1.0 200 OK\r\n"
				    "Content-length: %d\r\n"
				   "Connection: close\r\n"
					"Content-Type: text/html\r\n"
					"\r\n""%s";
	cs.WritePacket (templ);

	///while(1)
	{
	//	cout << "1";
	}

	return 0;
}
