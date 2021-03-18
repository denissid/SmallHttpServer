#include "Worker.h"

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <atomic>

std::atomic<bool> keep_thread_running;

void Worker (const ThreadSafeStack& stack, const std::string& directory)
{	
	try
	{
	    keep_thread_running = true;

        using namespace std;
        do
        {
	        if (keep_thread_running == false)
                break;

            int socket = stack.GetSocket();
            if (socket==-1)
                continue;

            ClientSocket cs(socket);

            Buffer buffer = cs.ReadPacket();

            WriteLog ("Get command from client");

            Packet packet = HTTPPacket::Parse (buffer);
            if (packet.IsGETMethod())
            {
                string path = packet.GetPath();
                string fullPath = directory + path;
                cout << "open ' " << fullPath << "' " << endl;

                fstream file (fullPath.c_str(), std::fstream::in);
                if (file)
                {

                    stringstream body;
                    body << file.rdbuf();

                    Buffer message = HTTPResponses::Create200(body.str());
                    cs.WritePacket (message);

                    WriteLog("Send message 200");
                }
                else
                {
                    cout << "File wasn't founded "<< fullPath << endl;
                    Buffer bufferError = HTTPResponses::Create404();

                    cs.WritePacket (bufferError);
                    
                    WriteLog("Send message 404");
                    WriteLog(HTTPPacket::Split(bufferError));
                }
            }
            else
            {
                cout << "Can't process command " << endl;
                Buffer bufferError = HTTPResponses::Create404();

                cs.WritePacket (bufferError);

                WriteLog("Send message 404");
                WriteLog (HTTPPacket::Split(bufferError));
            }
        
        }
        while(keep_thread_running);

	}
	catch (const std::exception& exception)
	{
		WriteLog (exception.what());
	}
	catch (...)
	{
		WriteLog ("Thread exception");
	}

}
