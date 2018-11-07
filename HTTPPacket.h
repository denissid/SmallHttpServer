#pragma once

#include <string>
#include <map>
#include <vector>

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

	std::string GetPath() const
	{
		return params.at("path");
	}
	
	bool IsGET() const 
	{
		return std::string("GET")==params.at("command"); 
	}
};

class HTTPPacket
{
	public:

		static Packet Parse (const Buffer& buffer);
		static std::vector<std::string> Split (const Buffer& buffer);
		static Buffer CreatePost200 (const std::string& dataFile);
		static Buffer CreatePost404 ();

};
