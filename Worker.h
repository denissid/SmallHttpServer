#pragma once

#include "ClientSocket.h"
#include "HTTPPacket.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void Worker (int socket, const std::string& directory)
{	

	using namespace std;

	ClientSocket cs(socket);

	Buffer buffer;
	cs.ReadPacket(buffer);
	cout << buffer;

	Packet packet;
	HTTPPacket::Parse (buffer,packet);
	if (packet.IsGET())
	{
		string path = packet.GetPath();
		string fullPath = directory + path;

		fstream file (fullPath.c_str(), std::fstream::in);
		if (file)
		{
			Buffer errMessage;
			stringstream body;
			body << file.rdbuf();
			HTTPPacket::CreatePost200(body.str(),errMessage);
			cs.WritePacket (errMessage);
			return;
		}
		else
		{
			cout << "File wasn't founed "<< fullPath << endl;
		}
	}

	Buffer bufferError;
	HTTPPacket::CreatePost404(bufferError);
	cs.WritePacket (bufferError);

}
