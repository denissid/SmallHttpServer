#pragma once

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "ThreadSafeStack.h"
#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

void Worker (const ThreadSafeStack& stack, const std::string& directory)
{	
	try
	{
	
	using namespace std;
	do
	{
		int socket = stack.GetSocket();
			
		ClientSocket cs(socket);

		Buffer buffer;
		cs.ReadPacket(buffer);
		cout << buffer;

		Packet packet = HTTPPacket::Parse (buffer);
		if (packet.IsGET())
		{
			string path = packet.GetPath();
			string fullPath = directory + path;
			cout << fullPath <<endl;
			fstream file (fullPath.c_str(), std::fstream::in);
			if (file)
			{
				stringstream body;
				body << file.rdbuf();

				Buffer errMessage = HTTPPacket::CreatePost200(body.str());
				cs.WritePacket (errMessage);
			}
			else
			{
				cout << "File wasn't founded "<< fullPath << endl;
				Buffer bufferError = HTTPPacket::CreatePost404();
				cs.WritePacket (bufferError);
				cout << bufferError << endl;
			}
		}
		else
		{
			Buffer bufferError = HTTPPacket::CreatePost404();
			cs.WritePacket (bufferError);
		}
	
	}
	while(1);

	}
	catch (const std::exception& exception)
	{
		WriteLog (std::string("Error packet ")+ exception.what());
	}
	catch (...)
	{
		WriteLog ("Thread exception");
	}

}
