// WSClientSim.cpp : This file contains the 'main' function. Program execution begins and ends there.

/// Mandatory header file(s) for Windows
#include "pch.h"

/// Header files
#include <iostream>
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/Net/HTTPMessage.h"
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include "Poco/ThreadPool.h"

/// Header files for File/Path handling
#include "Poco/Path.h"
#include "Poco/File.h"

/// Header files required for Date, Time handling
#include "Poco/LocalDateTime.h"
#include "Poco/DateTime.h"
#include "Poco/Timestamp.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/DateTimeFormat.h"

/// Header file reqd. for detecting OS Platform
#include "Poco/Foundation.h"

#include<fstream>

using Poco::Net::WebSocket;
using Poco::Net::HTTPClientSession;
using Poco::Net::HTTPRequest;
using Poco::Net::HTTPResponse;
using Poco::Net::HTTPMessage;

using Poco::Path;
using Poco::File;
using Poco::DateTimeFormatter;
using Poco::DateTimeFormat;
using Poco::LocalDateTime;

class multiWebSocketClient : public Poco::Runnable
{
public:

	static int iClientCount;

	virtual void run()
	{
		HTTPClientSession oClientSession("127.0.0.1", 9980);
		HTTPRequest request(HTTPMessage::HTTP_1_1);
		HTTPResponse response;

		try
		{
			/// Create WS Client instance & message to be sent from client
			WebSocket* oWSClient = new WebSocket(oClientSession, request, response);

			std::cout << "Client number: " << ++iClientCount << std::endl;
			const int BUFFER_SIZE = 256;

			/// Send message to client
			const char *sMessage = "Hello WebSocket!";
			int iBytesSent = oWSClient->sendFrame(sMessage, strlen(sMessage), WebSocket::FRAME_TEXT);
			std::cout << "Bytes sent: " << iBytesSent << std::endl;

			/// Create & log current local timestamp
			LocalDateTime oNow;
			std::string sNowTimestamp = DateTimeFormatter::format(oNow, DateTimeFormat::RFC1123_FORMAT);

			// Generates timestamp to be appended to the stream output file
			std::string sOutputFileTimestamp = DateTimeFormatter::format(oNow, "%e-%m-%Y_%H%M%S%F");

			/// Construct output stream for writing buffer received to file
			// TeeOutputStream tee(std::cout); // Uncomment if you wish to use a 
											// single stream object to write to both console & to file simultaneously

			// Set output file name
			// Create output file path & corresponding directories
			Path pOutputFilePath(false);
			pOutputFilePath.pushDirectory("output");
			File pOutputFile(pOutputFilePath);
			pOutputFile.createDirectories();

			// Name of the file buffer contents received are to be written to
			std::string outFileName("output" + sOutputFileTimestamp + ".raw");

			// Set the output File name
			pOutputFilePath.setFileName(outFileName);

			/// Create output file path based on the OS
#if defined(POCO_OS_FAMILY_WINDOWS)
			std::string sOutputFileName(pOutputFilePath.toString(Path::PATH_WINDOWS));
#elif defined(POCO_OS_FAMILY_UNIX)
			std::string sOutputFileName(pOutputFilePath.toString(Path::PATH_UNIX));
#endif

			std::ofstream outStream(sOutputFileName);

			/// Receive message from server
			char acRecvBuffer[BUFFER_SIZE];
			int iFlags = 0;
			int iBytesRecvd = oWSClient->receiveFrame(acRecvBuffer, BUFFER_SIZE, iFlags);
			std::cout << "Bytes received: " << iBytesRecvd << std::endl;
			// std::cout << acRecvBuffer << std::endl;
			outStream << acRecvBuffer << std::endl;

			oWSClient->close();
		}
		catch (const std::exception& e)
		{
			std::cout << "Exception" << e.what();
		}
	}
};

int multiWebSocketClient::iClientCount = 0;

int main(int args, char **argv)
{
	/*
	HTTPClientSession oClientSession("127.0.0.1", 9980);
	HTTPRequest request(HTTPMessage::HTTP_1_1);
	// request.set()
	HTTPResponse response;
	*/

	/*
	try
	{
		/// Create WS Client instance & message to be sent from client
		WebSocket* oWSClient = new WebSocket(oClientSession, request, response);

		const int BUFFER_SIZE = 256;
		
		/// Send message to client
		const char *sMessage = "Hello WebSocket!";
		int iBytesSent = oWSClient->sendFrame(sMessage, strlen(sMessage), WebSocket::FRAME_TEXT);
		std::cout << "Bytes sent: " << iBytesSent << std::endl;

		/// Receive message from server
		char acRecvBuffer[BUFFER_SIZE];
		int iFlags = 0;
		int iBytesRecvd = oWSClient->receiveFrame(acRecvBuffer, BUFFER_SIZE, iFlags);
		std::cout << "Bytes received: " << iBytesRecvd << std::endl;
		std::cout << acRecvBuffer << std::endl;

		oWSClient->close();
	}
	catch (const std::exception& e)
	{
		std::cout << "Exception" << e.what();
	}
	*/
	
	Poco::ThreadPool::defaultPool().addCapacity(100);

	multiWebSocketClient oClient;

	for (int i = 0; i < 100; i++)
	{
		Poco::ThreadPool::defaultPool().start(oClient);
	}
	Poco::ThreadPool::defaultPool().joinAll();

	return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
