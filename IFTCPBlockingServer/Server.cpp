#include "Server.h"
#include <iostream>
//#include <IFTCP\Helpers.h>

bool Server::Initialise(IPEndpoint endPoint)
{
	mMasterFDs.clear();
	mConnections.clear();
	if (Network::Initialise()) {
		std::cout << "initialised\n";
		
		mListeningSocket = Socket(endPoint.GetIPVersion());

		if (mListeningSocket.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			
			if (mListeningSocket.Listen(endPoint, 5) == IFTCP::PResult::P_Success) {//only local machine can connect
				std::cout << "socket successfully listening\n";
				WSAPOLLFD listeningSocketFD = {};
				listeningSocketFD.fd = mListeningSocket.GetSocketHandle();
				listeningSocketFD.events = POLLRDNORM;
				listeningSocketFD.revents = 0;
				mMasterFDs.push_back(listeningSocketFD);
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
	std::vector<WSAPOLLFD> tempFDs = mMasterFDs;
	if (WSAPoll(tempFDs.data(), tempFDs.size(), mAcceptTimeOut) > 0) {

		WSAPOLLFD listeningSocketFD = tempFDs[0];//listening socket

		IFTCP::Socket newConnectionSocket;
		IPEndpoint newConnectionEndPoint;
		if (listeningSocketFD.revents & POLLRDNORM) {
			
			if (mListeningSocket.Accept(newConnectionSocket,&newConnectionEndPoint) == IFTCP::PResult::P_Success) {
				mConnections.emplace_back(TCPConnection(newConnectionSocket, newConnectionEndPoint));
				TCPConnection& acceptedConnection= mConnections[mConnections.size() - 1];
				std::cout << acceptedConnection.ToString() + " New Connection accepted\n";
				WSAPOLLFD newConnectionSocketFD = {};
				newConnectionSocketFD.fd = newConnectionSocket.GetSocketHandle();
				newConnectionSocketFD.events = POLLRDNORM;
				newConnectionSocketFD.revents = 0;
				mMasterFDs.push_back(newConnectionSocketFD);
				//acceptedConnection.Close();
			}
			else {
				std::cout << "Failed to accept new connection\n";
			}
		}
	}
	//connected sockets
	for (int i = 1; i < tempFDs.size(); i++) {
		int connectionIndex = i - 1;
		TCPConnection& connection = mConnections[connectionIndex];
		if (tempFDs[i].revents & POLLERR) {
			std::cout << "Poll error occured on " << connection.ToString() << std::endl;
			mMasterFDs.erase(mMasterFDs.begin() + i);
			tempFDs.erase(tempFDs.begin() + i);
			connection.Close();
			mConnections.erase(mConnections.begin() + connectionIndex);
			i -= 1;
			continue;
		}

		if (tempFDs[i].revents & POLLHUP) {
			std::cout << "Poll hang up occured on " << connection.ToString() << std::endl;
			mMasterFDs.erase(mMasterFDs.begin() + i);
			tempFDs.erase(tempFDs.begin() + i);
			connection.Close();
			mConnections.erase(mConnections.begin() + connectionIndex);
			i -= 1;
			continue;
		}

		if (tempFDs[i].revents & POLLNVAL) {
			std::cout << "Poll invalid socket occured on " << connection.ToString() << std::endl;
			mMasterFDs.erase(mMasterFDs.begin() + i);
			tempFDs.erase(tempFDs.begin() + i);
			connection.Close();
			mConnections.erase(mConnections.begin() + connectionIndex);
			i -= 1;
			continue;
		}

		if (tempFDs[i].revents & POLLRDNORM) {
			char buffer[IFTCP::gMaxPacketSize];
			int bytesReceived = 0;
			bytesReceived = recv(tempFDs[i].fd, buffer, gMaxPacketSize, 0);
			//
			if (bytesReceived == 0) {
				std::cout << "[Recv == 0] Connection was lost on " << connection.ToString() << std::endl;
				mMasterFDs.erase(mMasterFDs.begin() + i);
				tempFDs.erase(tempFDs.begin() + i);
				connection.Close();
				mConnections.erase(mConnections.begin() + connectionIndex);
				i -= 1;
			}

			if (bytesReceived == SOCKET_ERROR) {
				int error = WSAGetLastError();
				if (error != WSAEWOULDBLOCK) {
					std::cout << "[Recv < 0] Connection was lost on " << connection.ToString() << std::endl;
					mMasterFDs.erase(mMasterFDs.begin() + i);
					tempFDs.erase(tempFDs.begin() + i);
					connection.Close();
					mConnections.erase(mConnections.begin() + connectionIndex);
					i -= 1;
				}				
			}

			if (bytesReceived > 0) {
				std::cout << " Message of size " << bytesReceived << " received from " << connection.ToString() << std::endl;
			}
		}
	}
}
