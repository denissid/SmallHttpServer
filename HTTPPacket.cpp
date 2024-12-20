#include "HTTPPacket.h"

#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <sstream>

using namespace std;

namespace HTTPPacket
{

string ExtractField(const Buffer& buffer, const string &field)
{
    auto bCommand = buffer.find (field);
    if (bCommand==string::npos)
        return {};

	auto bValue = buffer.find(" ", bCommand);
	bValue = buffer.find_first_not_of(" ", bValue);

    auto eValue = buffer.find("\r\n", bValue);
    if (eValue==string::npos)
    {
        eValue = buffer.size();
    }

    return buffer.substr(bValue, eValue-bValue);
}

bool IsBlankLine(const int offset, const Buffer &buffer)
{ 
    static const string endPacket = "\r\n\r\n";
    auto eSize = endPacket.size();
    if (offset>=eSize)
    {
        auto l = buffer.substr(offset-eSize, eSize);
        return l==endPacket;
    }
    return false;
}


Packet Parse (const Buffer& buffer)
{
	Packet packet;

 //   std::cout << buffer; 
//	WriteLog("Parse ");
//	WriteLog(HTTPPacket::Split(buffer));

    //parse header example: GET /index.html HTTP/1.1
	size_t i = buffer.find("\r\n");

	if (i==string::npos)
	{
		throw ParseException("\r\n wasn't found");
	}

	size_t b = 0;
	string header = buffer.substr(b, i-b);

	size_t k = header.find(" ");
	if (k==string::npos)
	{
		throw ParseException("doesn't find ' '");
	}

    //extract commands (GET, POST ... ) 
	string typeCommand = header.substr(b, k-b);

	packet.AddParam("command", typeCommand);
	
	k++;//move after space 
	size_t z = header.find_first_of(" ?", k);

	if (z==string::npos)
	{
		throw ParseException("doesn't find ' ?'");
	}

    //extract path 'index.html'
	string path = header.substr(k, z-k);
	packet.AddParam("path", path);

    const std::string nameField = "Connection";
    string valueConnection = ExtractField(buffer, nameField);
    packet.AddParam(nameField, valueConnection);

	return packet;
}

vector<string> Split (const Buffer& buffer)
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
	
    return splitted;
}

}

inline std::string getKeepAliveString(bool isKeepAlive)
{
    return isKeepAlive ? "keep-alive" : "close";
}


namespace HTTPRequest 
{
        Buffer CreateGET (const string& host, const string& port)
        {
            Buffer b = "GET /index.html HTTP/1.1\r\n"
            "Host: "+host+":"+port+"\r\n"
            "User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0\r\n"
            "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8\r\n"
            "Accept-Language: en-US,en;q=0.5\r\n"
            "Accept-Encoding: gzip, deflate\r\n"
            "Connection: keep-alive\r\n"
            "\r\n";
            return b;
        }
}


namespace HTTPResponses
{

Buffer Create200(const string& dataFile, const string &contentType, bool isKeepAlive)
{
	Buffer buffer ("HTTP/1.0 200 OK\r\n");

	buffer += "Content-length: " + to_string(dataFile.size()) + "\r\n"
	+ "Connection: "+getKeepAliveString(isKeepAlive)+"\r\n"
	+ "Content-Type: "+contentType +"\r\n"
	+ "\r\n"
	+ dataFile
    + "\r\n\r\n";
 

	return buffer;
}

Buffer Create400(bool isKeepAlive)
{
    string buffer = "HTTP/1.0 400 Bad Request\r\n";
    buffer += "Connection: "+getKeepAliveString(isKeepAlive)+"\r\n";
    buffer += "Content-Length: 11\r\n\r\nBad Request\r\n\r\n";

    return buffer;
}

Buffer Create404 (bool isKeepAlive)
{
	
    string body = "<html> <head> <title>Not Found</title> </head> <body> <p>404 Request file not found.</p></body></html>";
    string buffer ("HTTP/1.0 404 Not Found\r\n");
		buffer += "Content-Type: text/html\r\n";
	   	buffer += "Content-length: " + to_string(body.size()) + "\r\n";
		buffer += "Connection: "+getKeepAliveString(isKeepAlive)+"\r\n\r\n" + body+"\r\n\r\n";

	return buffer;
}

}

namespace FileHelper 
{

const  std::string GetContentType(const std::string &fileName)
{ 
    static std::map<std::string,std::string> files = {{".css", "text/css"},
                                           {".csv","text/csv"},
                                           {".gif","image/gif"},
                                           {".htm","text/html"},
                                           {".html","text/html"},
                                           {".ico","image/x-icon"},
                                           {".jpeg","image/jpeg"},
                                           {".jpg","image/jpeg"},
                                           {".js","application/javascript"},
                                           {".json", "application/json"},
                                           {".png","image/png"},
                                           {".pdf","application/pdf"},
                                           {".svg","application/svg+xml"},
                                           {".txt", "text/plain"} };


    auto pos = fileName.rfind('.');
    if (pos==std::string::npos)
    {
        logError("Error file name", fileName);
        return "";
    }
    auto ext = fileName.substr(pos, std::string::npos);
    return files.at(ext);
}

}
