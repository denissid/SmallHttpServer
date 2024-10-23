#pragma once

#include <vector>
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "Buffer.h"


	/*
     * example of packet
     *
	GET /index.html HTTP/1.1
	Host: 127.0.0.1:2000
	User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0
	Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8
	Accept-Language: en-US,en;q=0.5
	Accept-Encoding: gzip, deflate
	Connection: keep-alive
	*/
namespace HTTPParams 
{
    static const std::string commnad = "command";
    //static const std::string connection = "connection";
    static const std::string path = "path";
}

struct Packet
{
	std::map <std::string, std::string> params;

	void AddParam (const std::string& name, 
			const std::string& value)
	{
		params.insert (std::make_pair(name,value));
	}

	std::string GetPath() const
	{
		return params.at("path");
	}
	
	bool IsGETMethod() const 
	{
		return std::string("GET")==params.at("command"); 
	}

    bool IsKeepAlive() const 
    {
        std::string val = params.at("Connection");
        std::transform(begin(val), end(val), begin(val), [](char c)
                {
                    return tolower(c);
                });
        return std::string("keep-alive")==val;
    }
};

namespace HTTPPacket
{
        std::string ExtractField(const Buffer& buffer, const std::string& field);
        bool IsBlankLine(const int offset, const Buffer &buffer);
		Packet Parse (const Buffer& buffer);
		std::vector<std::string> Split (const Buffer& buffer);
}

namespace HTTPRequest 
{
        Buffer CreateGET (const std::string& host, const std::string& port);
}

namespace HTTPResponses
{
		Buffer Create200 (const std::string& dataFile, const std::string &contentType, bool isKeepAlive=true);

        Buffer Create400(bool isKeepAlive=false);
		Buffer Create404 (bool isKeepAlive=false);
}

namespace FileHelper 
{
    const  std::string GetContentType(const std::string &fileName);
}

