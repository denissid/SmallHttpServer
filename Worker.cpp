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
#include <format>

namespace Commander
{
    bool ProcessGET (auto &cs, const Packet &packet, const std::string &directory)
    { 
        using namespace std;

        string path = packet.GetPath();
        //check path 
        if (path.size()>100)
        {
            Buffer message = HTTPResponses::Create400();
            cs->WritePacket (message);
            return false;
        }
        if (path=="..")
        {
            Buffer message = HTTPResponses::Create404();
            cs->WritePacket (message);

            return false;
        }

        string fullPath = directory + path;

        auto contentType = FileHelper::GetContentType(fullPath);

        fstream file (fullPath.c_str(), std::fstream::in);
        if (!file)
        {
            Buffer message = HTTPResponses::Create404();
            cs->WritePacket (message);
            return false;
        }

        log("Open ' ", fullPath, "' ");
        stringstream body;
        body << file.rdbuf();

        Buffer message = HTTPResponses::Create200(body.str(), contentType, true);
        int result = cs->WritePacket (message);
        if (result<=0)
        { 
            Buffer bufferError = HTTPResponses::Create404();
            cs->WritePacket (bufferError);

            logError("Error of send response");
            return false;
        }

        return true;
    }
}

void Worker (std::stop_token st, const ThreadSafeStack& stack, const std::string& directory)
{	
	try
	{

        using namespace std;
        do
        {

         //   std::cout << "get socket" << std::endl;
            auto socket = stack.GetSocket(st);

            if (st.stop_requested())
                return;
            if (socket==nullptr)
                continue;

            socket->SetTimeout();
            bool isKA = true;
            while (isKA && !st.stop_requested()){

                stringstream ss;
                ss << std::this_thread::get_id();
                log("Thread id ", ss.str());
                //log("Thread id ", std::this_thread::get_id());

                if (!socket->IsAlive())
                {
                    logError("Socket is not alive");
                    break;
                }

                Buffer buffer;
                int result = socket->ReadPacket(buffer);
                if (result<=0)
                {
                    logError("Error result of read packet ",result);
                    break;
                }

                Packet packet = HTTPPacket::Parse(buffer);
                isKA = packet.IsKeepAlive();
                if (packet.IsGETMethod())
                {
                    bool isProcessed= Commander::ProcessGET (socket, packet, directory);
                    if (!isProcessed)
                    {
                       break; 
                    }
                }
            }

            log("Connection: Close");

        }
        while(!st.stop_requested());

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
