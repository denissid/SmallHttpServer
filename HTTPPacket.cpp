#include "HTTPPacket.h"

#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <sstream>

using namespace std;

Packet HTTPPacket::Parse (const Buffer& buffer)
{
	Packet packet;

	WriteLog("Parse "+buffer);

	size_t b = 0;
	size_t i = buffer.find("\r\n");

	if (i==string::npos)
	{
		throw ParseException("doesn't find \r\n");
	}

	string s = buffer.substr(b, i-b);

	size_t k = s.find(" ");
	if (k==string::npos)
	{
		throw ParseException("doesn't find ' '");
	}

	string typeCommand = s.substr(b, k-b);
	packet.AddParam("command", typeCommand);
	
	k = k+1;//move after space 
	size_t z = s.find_first_of(" ?",k);

	if(z==string::npos)
	{
		throw ParseException("doesn't find ' ?'");
	}

	string path = s.substr(k, z-k);

	packet.AddParam("path", path);

	return packet;
}

std::vector<std::string> HTTPPacket::Split (const Buffer& buffer)
{
	using namespace std;

	vector<string> splitted;

	if (buffer.empty())
	{
		return splitted;
	}

	string::size_type pos = 0;

	do
	{
		auto nextPos = buffer.find("\r\n", pos);
		
		if (nextPos == string::npos)
		{
			nextPos = buffer.size();
		}

		string t = buffer.substr(pos, nextPos-pos);
		splitted.push_back(t);
		
		if (nextPos==buffer.size())
		{
			return splitted;
		}

		//move after \r\n
		pos = nextPos + 2;
	}
	while(pos<buffer.size());
	
}

Buffer HTTPPacket::CreatePost200(const std::string& dataFile)
{
	Buffer buffer ("HTTP/1.0 200 OK\r\n");

	buffer += "Content-length: " + to_string(dataFile.size()) + "\r\n"
	+ "Connection: close\r\n"
	+ "Content-Type: text/html\r\n"
	+ "\r\n"
	+ dataFile;
 

	return buffer;
}

Buffer HTTPPacket::CreatePost404 ()
{
	
	string body = "<html> <head> <title>Not Found</title> </head> <body> <p>404 Request file not found.</p></body></html>";
	string buffer ("HTTP/1.0 404 Not Found\r\n");
		buffer += "Content-Type: text/html\r\n";
	   	buffer += "Content-length: " + to_string(body.size()) + "\r\n";
		buffer += "Connection: close\r\n" + body;

	return buffer;
}
