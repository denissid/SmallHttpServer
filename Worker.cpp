#include "Worker.h"

#include "ClientSocket.h"
#include "HTTPPacket.h"
#include "Logger.h"
#include "Exceptions.h"
#include "ThreadSafeStack.h"

#include <thread>
#include <iostream>
#include <fstream>
#include <sstream>

namespace Commander
{
    bool ProcessGET (const ClientSocket &cs, const Packet &packet, const std::string &directory)
    { 
        using namespace std;

        string path = packet.GetPath();
        //check path 
        if (path.size()>100)
        {
            Buffer message = HTTPResponses::Create400();
            cs.WritePacket (message);
            return false;
        }
        if (path=="..")
        {
            Buffer message = HTTPResponses::Create404();
            cs.WritePacket (message);

            return false;
        }

        string fullPath = directory + path;

        auto contentType = FileHelper::GetContentType(fullPath);

        fstream file (fullPath.c_str(), std::fstream::in);
        if (!file)
        {
            Buffer message = HTTPResponses::Create404();
            cs.WritePacket (message);
            return false;
        }

        cout << "Open ' "+ fullPath + "' " << endl;
        stringstream body;
        body << file.rdbuf();

        Buffer message = HTTPResponses::Create200(body.str(), contentType);
        int result = cs.WritePacket (message);
        if (result<=0)
        { 
            Buffer bufferError = HTTPResponses::Create404();
            cs.WritePacket (bufferError);

            LogError() << "Error of send response" << endl;
            return false;
        }

        return true;
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
            cs.SetTimeout();

            do
            {
                stringstream ss;
                ss << std::this_thread::get_id();
                Log() << "Thread id " + ss.str() << std::endl;

                Buffer buffer;
                int result = cs.ReadPacket(buffer);
                if (result<=0)
                {
                    LogError() << "Error result of read packet "+ to_string(result) << std::endl;
                    break;
                }

                Packet packet = HTTPPacket::Parse (buffer);
                isKeepAlive = packet.IsKeepAlive();

                if (packet.IsGETMethod())
                {
                    bool isProcessed= Commander::ProcessGET (cs, packet, directory);
                    if (!isProcessed)
                    {
                       break; 
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
