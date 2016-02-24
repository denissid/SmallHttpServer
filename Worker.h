#pragma once

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "ThreadSafeStack.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void Worker (const ThreadSafeStack& stack, const std::string& directory)
{	

	using namespace std;

	do
	{
		int socket = stack.GetSocket();
		

		{
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
			cout << fullPath <<endl;
			fstream file (fullPath.c_str(), std::fstream::in);
			if (file)
			{
				Buffer errMessage;
				stringstream body;
				body << file.rdbuf();
				HTTPPacket::CreatePost200(body.str(),errMessage);
				cs.WritePacket (errMessage);
			}
			else
			{
				cout << "File wasn't founed "<< fullPath << endl;
				Buffer bufferError;
				HTTPPacket::CreatePost404(bufferError);
				cs.WritePacket (bufferError);
			}
		}
		else
		{
			Buffer bufferError;
			HTTPPacket::CreatePost404(bufferError);
			cs.WritePacket (bufferError);
		}
		}
	}
	while(1);

}
