#include "HTTPPacket.h"
#include <iostream>
#include <sstream>

using namespace std;

void HTTPPacket::Parse (Buffer& buffer, Packet& packet)
{
	{
	size_t b = 0;
	size_t i = buffer.find("\r\n");
	string s = buffer.substr(b, i-b);

	size_t k = s.find(" ");
	string typeCommand = s.substr(b, k-b);
	//cout <<"type command "<< typeCommand << endl;
	packet.AddParam("command", typeCommand);
	
	k = k+1;//move after space 
	size_t z = s.find_first_of(" ?",k);
	string path = s.substr(k, z-k);
	//cout << "path " << path << endl;

	packet.AddParam("path", path);
	}

/*	size_t i = string::npos, b = 0;
	do 
	{
		i = buffer.find("\r\n");
		cout << s << endl;
	}
	while (i!=string::npos);
*/	
}


void HTTPPacket::CreatePost404 (Buffer& buffer)
{
	static const std::string  not_found = "Content-Type: text/html\r\n"
						"Content-length: 107\r\n"
						"Connection: close\r\n"
						"<html> <head> <title>Not Found</title> </head> <body> <p>404 Request file not found.</p>"
						"</body></html>";

	buffer = not_found;
}

void HTTPPacket::CreatePost200(const std::string& dataFile, 
				Buffer& buffer)
{
	stringstream ss;
	ss << "HTTP/1.0 200 OK\r\n"
	   << "Content-length: "<<dataFile.size()<<"\r\n"
	   << "Connection: close\r\n"
	   << "Content-Type: text/html\r\n"
	   << "\r\n"
	   << dataFile; 

	buffer = ss.str();
}


