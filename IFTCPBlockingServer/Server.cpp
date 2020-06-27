#include "Server.h"
#include <iostream>
//#include <IFTCP\Helpers.h>

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
	WSAPOLLFD listeningSocketFD = {};
	listeningSocketFD.fd = mListeningSocket.GetSocketHandle();
	listeningSocketFD.events = POLLRDNORM;
	listeningSocketFD.revents = 0;
	if (WSAPoll(&listeningSocketFD, 1, 2) > 0) {
		IFTCP::Socket newConnection;
		IPEndpoint newConnectionEndPoint;
		if (listeningSocketFD.revents & POLLRDNORM) {
			
			if (mListeningSocket.Accept(newConnection,&newConnectionEndPoint) == IFTCP::PResult::P_Success) {
				TCPConnection acceptedConnection(newConnection, newConnectionEndPoint);
				std::cout << acceptedConnection.ToString() + " New Connection accepted\n";

				acceptedConnection.Close();
			}
			else {
				std::cout << "Failed to accept new connection\n";
			}
		}
	} //1ms timeout
	
}
