#include "HTTPPacket.h"

#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <sstream>

using namespace std;

void HTTPPacket::Parse (Buffer& buffer, Packet& packet)
{
	{
	
	WriteLog("Parse "+buffer);

	size_t b = 0;
	size_t i = buffer.find("\r\n");
	if (i==string::npos)
		throw ParseException("doesn't find \r\n");

	string s = buffer.substr(b, i-b);

	size_t k = s.find(" ");
	if (k==string::npos)
		throw ParseException("doesn't find ' '");

	string typeCommand = s.substr(b, k-b);
	packet.AddParam("command", typeCommand);
	
	k = k+1;//move after space 
	size_t z = s.find_first_of(" ?",k);
	if(z==string::npos)
		throw ParseException("doesn't find ' ?'");
	string path = s.substr(k, z-k);

	packet.AddParam("path", path);
	}
}


void HTTPPacket::CreatePost404 (Buffer& buffer)
{
	string body = "<html> <head> <title>Not Found</title> </head> <body> <p>404 Request file not found.</p></body></html>";
	stringstream ss;
	ss << "HTTP/1.0 404 Not Found\r\n"
	   << "Content-Type: text/html\r\n"
	   <<  "Content-length: " << body.size() <<"\r\n"
	   << "Connection: close\r\n" << body;
	buffer = ss.str();
	WriteLog("Create 404 "+buffer);
}

void HTTPPacket::CreatePost200(const std::string& dataFile, 
				Buffer& buffer)
{
	stringstream ss;
	ss << "HTTP/1.0 200 OK\r\n"
	   << "Content-length: " << dataFile.size() << "\r\n"
	   << "Connection: close\r\n"
	   << "Content-Type: text/html\r\n"
	   << "\r\n"
	   << dataFile; 

	buffer = ss.str();
	WriteLog("Create 200 "+buffer);
}


