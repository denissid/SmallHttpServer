#pragma once

#include "HTTPPacket.h"
#include <cassert>

void TestGETParse()
{	
	Buffer buffer = "GET /index.html?a=5 HTTP/1.1\r\n"
			"Host: 127.0.0.1:2000\r\n"
			"User-Agent: Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:35.0) Gecko/20100101 Firefox/35.0\r\n"
			"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*//*;q=0.8\r\n"
			"Accept-Language: en-US,en;q=0.5\r\n"
			"Accept-Encoding: gzip, deflate\r\n"
			"Connection: keep-alive\r\n";
	Packet packet;
	HTTPPacket::Parse (buffer,packet);
	std::string path = packet.GetPath();
	assert (path=="/index.html");
	assert (packet.IsGET() == true);
}


