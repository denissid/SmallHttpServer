#include "Worker.h"

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "Logger.h"
#include "Exceptions.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <atomic>

namespace Commander
{
    bool ProcessGET (const ClientSocket &cs, const Packet &packet, const std::string &directory)
    { 
        using namespace std;

        string path = packet.GetPath();
        string fullPath = directory + path;
        cout << "try open ' " << fullPath << "' " << endl;

        fstream file (fullPath.c_str(), std::fstream::in);
        if (file)
        {

            stringstream body;
            body << file.rdbuf();

            Buffer message = HTTPResponses::Create200(body.str());
            cs.WritePacket (message);

            WriteLog("Send message 200");
            return true;
        }
        else
        {
            Log() << "file is absened " << path << std::endl;
        }
        return false;
    }
}

std::atomic<bool> keepThreadRunning;

void Worker (const ThreadSafeStack& stack, const std::string& directory)
{	
	try
	{
	    keepThreadRunning = true;


        using namespace std;
        do
        {
            bool isKeepAlive = false;
	        if (keepThreadRunning == false)
                break;

            int socket = stack.GetSocket();
            if (socket==-1)
                continue;

            ClientSocket cs(socket);

            WriteLog ("Get command from client");
            Log() << "Thread id = " << std::this_thread::get_id() << std::endl;

            do
            {
                Buffer buffer = cs.ReadPacket();
                if (buffer.empty())
                    break;

                Packet packet = HTTPPacket::Parse (buffer);
                isKeepAlive = packet.IsKeepAlive();
                if (isKeepAlive)
                    std::cout << " keep alive " << std::endl;
//need to check
                if (packet.IsGETMethod())
                {
                    bool isOk = Commander::ProcessGET (cs, packet, directory);
                    if (!isOk)
                    { 
                        cout << "Can't process command (only GET supported)" << endl;
                        Buffer bufferError = HTTPResponses::Create404();

                        cs.WritePacket (bufferError);

                        WriteLog("Send message 404");
                        WriteLog (HTTPPacket::Split(bufferError));
                    }
                }
            }
            while (isKeepAlive && keepThreadRunning);
        
        }
        while(keepThreadRunning);

	}
	catch (const std::exception& e)
	{
		WriteLog (e.what());
	}
	catch (...)
	{
		WriteLog ("Thread exception");
	}

}
