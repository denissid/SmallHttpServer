#pragma once

#include "HTTPPacket.h"
#include <cassert>
#include <vector>
#include <string>
#include <map>

Buffer buffer = "GET /index.html?a=5 HTTP/1.1\r\n"
		"Host: 127.0.0.1:2000\r\n"
		"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0\r\n"
		"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8\r\n"
		"Accept-Language: en-US,en;q=0.5\r\n"
		"Accept-Encoding: gzip, deflate\r\n"
		"Connection: keep-alive\r\n";

void TestGETParse()
{	
	using namespace std;
    using namespace HTTPPacket;

	Packet packet = Parse (buffer);
	string path = packet.GetPath();
	assert (path=="/index.html");
	assert (packet.IsGETMethod() == true);
}

void TestExtract()
{
    auto field = HTTPPacket::ExtractField(buffer, "Connection");
    std::cout << "'" << field << "'";
    assert (field=="keep-alive");
}

void TestSplit()
{
	using namespace std;
    using namespace HTTPPacket;

	auto vec = Split (buffer);
	assert (vec.size()==7);

	Buffer bufferOneLine = "String line \r\n";
	auto v = Split (bufferOneLine);
	assert (v.size()==1);

	Buffer bufferTwoLine = "String line \r\n 12345";
	auto v1 = Split (bufferTwoLine);
	assert (v1.size()==2);

	Buffer bufferEndLine = "String line ";
	auto v2 = Split (bufferEndLine);
	assert (v2.size()==1);

}

void TestGetContentType()
{
    std::map<std::string,std::string> files = {{"file.css", "text/css"},
                                               {"file.csv","text/csv"},
                                               {"file.gif","image/gif"},
                                               {"file.htm","text/html"},
                                               {"file.html","text/html"},
                                               {"file.ico","image/x-icon"},
                                               {"file.jpeg","image/jpeg"},
                                               {"file.jpg","image/jpeg"},
                                               {"file.js","application/javascript"},
                                               {"file.json", "application/json"},
                                               {"file.png","image/png"},
                                               {"file.pdf","application/pdf"},
                                               {"file.svg","application/svg+xml"},
                                               {"file.txt", "text/plain"} };

    for (auto &t: files)
    {
        auto res = FileHelper::GetContentType(t.first);
        assert(res==t.second);
    }
}
