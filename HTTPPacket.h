#pragma once

#include <string>
#include <map>

typedef std::string Buffer;


struct Packet
{
/*
GET /index.html HTTP/1.1
Host: 127.0.0.1:2000
User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
*/
	std::map <std::string, std::string > params;

	void AddParam (const std::string& name, 
			const std::string& value)
	{
		params.insert (std::make_pair(name,value));
	}

	std::string GetPath()
	{
		return params["path"];
	}
	
	bool IsGET() 
	{
		return std::string("GET")==params["command"]; 
	}
};

class HTTPPacket
{
	public:

		static void Parse (Buffer& buffer, Packet& packet);
		static void CreatePost200 (const std::string& dataFile, Buffer& buffer);
		static void CreatePost404 (Buffer& buffer);

};
