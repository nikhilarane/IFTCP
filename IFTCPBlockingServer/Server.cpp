#include "Server.h"
#include <iostream>

bool Server::Initialise(IPEndpoint endPoint)
{
	if (Network::Initialise()) {
		std::cout << "initialised\n";
		
		mListeningSocket = Socket(endPoint.GetIPVersion());

		if (mListeningSocket.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			
			if (mListeningSocket.Listen(endPoint, 5) == IFTCP::PResult::P_Success) {//only local machine can connect
				std::cout << "socket successfully listening\n";
				return true;					
			}
			else {
				std::cout << "Failed to listen\n";
			}
			if (mListeningSocket.Close() == IFTCP::PResult::P_Success) {
				std::cout << "Socket Succesfully Closed\n";
			}

		}
		else {
			std::cout << "Failed to create socket\n";
		}
				
	}
	return false;
}

void Server::Frame()
{
	IFTCP::Socket newConnection;
	if (mListeningSocket.Accept(newConnection) == IFTCP::PResult::P_Success) {
		std::cout << "socket successfully accepted\n";
		newConnection.Close();		
	}
	else {
		std::cout << "Failed to accept new connection\n";
	}
}
