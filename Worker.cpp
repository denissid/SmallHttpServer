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
#include <cassert>

namespace Commander
{
    Buffer CreateRespond(const Packet &packet, const std::string &directory)
    { 
        using namespace std;

        Buffer message; 
        string path = packet.GetPath();

        //check path 
        if (path.size()>100)
        {
            message = HTTPResponses::Create400();
            return message;
        }
        if (path=="..")
        {
            message = HTTPResponses::Create404();
            return message;
        }

        string fullPath = directory + path;

        auto contentType = FileHelper::GetContentType(fullPath);

        fstream file (fullPath.c_str(), std::fstream::in);
        if (!file)
        {
            message = HTTPResponses::Create404();
            return message;
        }

        log("Open ' ", fullPath, "' ");
        stringstream body;
        body << file.rdbuf();

        Packet::Connection connection = packet.IsKeepAlive();
        if (connection==Packet::Connection::KeepAlive)
        {
            logError("Packet contains keep-alive");
        }
        else
        if (connection==Packet::Connection::Close)
        {
            logError("Packet contains close connection");
        }
        else
        {
            logError("Packet contains connection empty");
        }

        message = HTTPResponses::Create200(body.str(), contentType, true);
        logError(message);

        return message;

        /*int result = socket->WritePacket (message);
        if (result<=0)
        { 
            Buffer bufferError = HTTPResponses::Create404();
            socket->WritePacket (bufferError);

            logError("Error of send response");
            return false;
        }

        return true;
        */

    }

    bool ProcessGET (auto &socket, const Packet &packet, const std::string &directory)
    { 
        using namespace std;

        string path = packet.GetPath();
        //check path 
        if (path.size()>100)
        {
            Buffer message = HTTPResponses::Create400();
            socket->WritePacket (message);
            return false;
        }
        if (path=="..")
        {
            Buffer message = HTTPResponses::Create404();
            socket->WritePacket (message);

            return false;
        }

        string fullPath = directory + path;

        auto contentType = FileHelper::GetContentType(fullPath);

        fstream file (fullPath.c_str(), std::fstream::in);
        if (!file)
        {
            Buffer message = HTTPResponses::Create404();
            socket->WritePacket (message);
            return false;
        }

        log("Open ' ", fullPath, "' ");
        stringstream body;
        body << file.rdbuf();

        Packet::Connection connection = packet.IsKeepAlive();
        if (connection==Packet::Connection::KeepAlive)
        {
            logError("Packet contains keep-alive");
        }
        else
        if (connection==Packet::Connection::Close)
        {
            logError("Packet contains close connection");
        }
        else
        {
            logError("Packet contains connection empty");
        }

        Buffer message = HTTPResponses::Create200(body.str(), contentType, true);
        logError(message);
        int result = socket->WritePacket (message);
        if (result<=0)
        { 
            Buffer bufferError = HTTPResponses::Create404();
            socket->WritePacket (bufferError);

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
            //std::cout << "get context" << std::endl;
            auto context = stack.GetContext(st);

            if (!context)
            {
                auto err = context.error();
                if (err==ThreadStackErr::TIMEOUT)
                    continue;
                else
                {
                    log("Close thread");
                    break;
                }
            }

            auto socket = (*context)->GetSocket();
            if (socket==nullptr)
                continue;

            socket->SetTimeout();
            bool isKA = true;
            while (isKA && !st.stop_requested()){

                stringstream ss;
                ss << std::this_thread::get_id();
                log("Thread id ", ss.str());

                if (!socket->IsAlive())
                {
                    logError("Socket is not alive");
                    break;
                }


                Buffer buffer;
                auto [size, result] = socket->ReadPacket(buffer, MaxBufferSize);
                if (result<=0)
                {
                    if (result<0 && errno==EAGAIN)
                    {
                        logError("We get EAGAIN on READ ");
                    }
                    
                    logError("Error result of read packet ",result);
                    break;
                }

                Packet packet = HTTPPacket::Parse(buffer);
                isKA = packet.IsKeepAlive()==Packet::Connection::Close ? false : true;
                if (packet.IsGETMethod())
                {
                    auto respond = Commander::CreateRespond (packet, directory);
                    auto [size, result]  = socket->WritePacket(respond, respond.size());
                    if (result<=0)
                    {
                        if (result<0 && errno==EAGAIN)
                        {
                            logError("We get EAGAIN on WRITE ");
                        }   
                    
                        logError("Error result of write packet ",result);
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
