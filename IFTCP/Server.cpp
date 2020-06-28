#include "Server.h"
#include <iostream>
#include "IFTCP.h"
#include "Helpers.h"

namespace IFTCP {


	

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
		mTempFDs = mMasterFDs;
		if (WSAPoll(mTempFDs.data(), mTempFDs.size(), mAcceptTimeOut) > 0) {

			WSAPOLLFD listeningSocketFD = mTempFDs[0];//listening socket

			IFTCP::Socket newConnectionSocket;
			IPEndpoint newConnectionEndPoint;
			if (listeningSocketFD.revents & POLLRDNORM) {

				if (mListeningSocket.Accept(newConnectionSocket, &newConnectionEndPoint) == IFTCP::PResult::P_Success) {
					mConnections.emplace_back(TCPConnection(newConnectionSocket, newConnectionEndPoint));
					TCPConnection& acceptedConnection = mConnections[mConnections.size() - 1];
					std::cout << acceptedConnection.ToString() + " New Connection accepted\n";
					WSAPOLLFD newConnectionSocketFD = {};
					newConnectionSocketFD.fd = newConnectionSocket.GetSocketHandle();
					newConnectionSocketFD.events = POLLRDNORM | POLLWRNORM;
					newConnectionSocketFD.revents = 0;
					mMasterFDs.push_back(newConnectionSocketFD);
					std::shared_ptr<Packet> welcomeMessage = std::make_shared<Packet>(PacketType::PT_ChatMessage);
					*welcomeMessage << "Welcome to Nikhil's world " << acceptedConnection.ToString();
					acceptedConnection.mPMOutGoing.Append(welcomeMessage);
					//acceptedConnection.Close();
				}
				else {
					std::cout << "Failed to accept new connection\n";
				}
			}
		}
		//connected sockets
		for (int i = mTempFDs.size() - 1; i > 0; i--) {
			int connectionIndex = i - 1;
			TCPConnection& connection = mConnections[connectionIndex];
			if (mTempFDs[i].revents & POLLERR) {
				CloseConnection(connectionIndex, "Poll error occured on ");
				continue;
			}

			if (mTempFDs[i].revents & POLLHUP) {
				CloseConnection(connectionIndex, "Poll hang up occured on ");
				continue;
			}

			if (mTempFDs[i].revents & POLLNVAL) {
				CloseConnection(connectionIndex, "Poll invalid socket occured on ");
				continue;
			}

			if (mTempFDs[i].revents & POLLRDNORM) {
				int bytesReceived = 0;
				if (connection.mPMIncoming.mCurrentTask == PacketManagerTask::ProcessPacketSize) {

					bytesReceived = recv(mTempFDs[i].fd, (char*)&connection.mPMIncoming.mCurrentPacketSize + connection.mPMIncoming.mCurrentPacketExtractionOffset, sizeof(uint16_t) - connection.mPMIncoming.mCurrentPacketExtractionOffset, 0);


				}
				else {
					bytesReceived = recv(mTempFDs[i].fd, (char*)&connection.mBuffer + connection.mPMIncoming.mCurrentPacketExtractionOffset, connection.mPMIncoming.mCurrentPacketSize - connection.mPMIncoming.mCurrentPacketExtractionOffset, 0);
				}


				//
				if (bytesReceived == 0) {
					CloseConnection(connectionIndex, "[Recv == 0] Connection was lost on ");

				}

				if (bytesReceived == SOCKET_ERROR) {
					int error = WSAGetLastError();
					if (error != WSAEWOULDBLOCK) {
						CloseConnection(connectionIndex, "[Recv < 0] Connection was lost on");

					}
				}

				if (bytesReceived > 0) {
					connection.mPMIncoming.mCurrentPacketExtractionOffset += bytesReceived;
					if (connection.mPMIncoming.mCurrentTask == PacketManagerTask::ProcessPacketSize) {
						if (connection.mPMIncoming.mCurrentPacketExtractionOffset == sizeof(uint16_t)) {
							connection.mPMIncoming.mCurrentPacketSize = ntohs(connection.mPMIncoming.mCurrentPacketSize);
							if (connection.mPMIncoming.mCurrentPacketSize > IFTCP::gMaxPacketSize) {
								CloseConnection(connectionIndex, "Packet size too large. Closing connection");
								continue;
							}
							connection.mPMIncoming.mCurrentPacketExtractionOffset = 0;
							connection.mPMIncoming.mCurrentTask = PacketManagerTask::ProcessPacketContents;
						}
					}
					else {
						if (connection.mPMIncoming.mCurrentPacketExtractionOffset == connection.mPMIncoming.mCurrentPacketSize) {
							//Packet p;
							//p.mBuffer.resize(connection.mPMIncoming.mCurrentPacketSize);
							//memcpy_s(&p.mBuffer[0], connection.mPMIncoming.mCurrentPacketSize, &connection.mBuffer[0], connection.mPMIncoming.mCurrentPacketSize);
							//std::cout << connection.ToString() << " : ";
							/*if (!ProcessPacket(p)) {
							CloseConnection(connectionIndex, "Failed to process Packet. Closing connection");
							continue;
							}*/
							std::shared_ptr<Packet> p = std::make_shared<Packet>();
							p->mBuffer.resize(connection.mPMIncoming.mCurrentPacketSize);
							memcpy_s(&(p->mBuffer[0]), connection.mPMIncoming.mCurrentPacketSize, connection.mBuffer, connection.mPMIncoming.mCurrentPacketSize);
							connection.mPMIncoming.Append(p);
							//std::cout << connection.ToString() << " : ";

							connection.mPMIncoming.mCurrentPacketSize = 0;
							connection.mPMIncoming.mCurrentPacketExtractionOffset = 0;
							connection.mPMIncoming.mCurrentTask = PacketManagerTask::ProcessPacketSize;
						}
					}
					//std::cout << " Message of size " << bytesReceived << " received from " << connection.ToString() << std::endl;
				}
			}

			if (mTempFDs[i].revents & POLLWRNORM) {
				PacketManager& pm = connection.mPMOutGoing;

				while (pm.HasPendingPackets()) {
					if (pm.mCurrentTask == PacketManagerTask::ProcessPacketSize) {
						pm.mCurrentPacketSize = pm.Retrieve()->mBuffer.size();
						uint16_t bigEndianPacketSize = htons(pm.mCurrentPacketSize);
						int bytesSent = send(mTempFDs[i].fd, (char*)&bigEndianPacketSize + pm.mCurrentPacketExtractionOffset, sizeof(uint16_t) - pm.mCurrentPacketExtractionOffset, 0);
						//or you can also use
						//int bytesSent = send(connection.mSocket.GetSocketHandle(), (char*)&bigEndianPacketSize  + pm.mCurrentPacketExtractionOffset, sizeof(uint16_t)  - pm.mCurrentPacketExtractionOffset, 0);
						if (bytesSent > 0) {
							pm.mCurrentPacketExtractionOffset += bytesSent;
						}
						if (bytesSent == sizeof(uint16_t)) {
							pm.mCurrentPacketExtractionOffset = 0;
							pm.mCurrentTask = PacketManagerTask::ProcessPacketContents;
						}
						else {
							break;//get in the next call to the Frame function
						}
					}
					else {
						char *bufferPtr = &pm.Retrieve()->mBuffer[0];
						int bytesSent = send(mTempFDs[i].fd, (char*)bufferPtr + pm.mCurrentPacketExtractionOffset, pm.mCurrentPacketSize - pm.mCurrentPacketExtractionOffset, 0);
						if (bytesSent > 0) {
							pm.mCurrentPacketExtractionOffset += bytesSent;
						}
						if (bytesSent == pm.mCurrentPacketSize) {
							pm.mCurrentPacketExtractionOffset = 0;
							pm.mCurrentTask = PacketManagerTask::ProcessPacketSize;
							pm.Pop();
						}
						else {
							break;//get in the next call to the Frame function
						}
					}
				}

			}
		}

		for (int i = mConnections.size() - 1; i >= 0; i--) {
			while (mConnections[i].mPMIncoming.HasPendingPackets()) {
				std::shared_ptr<Packet> frontPacket = mConnections[i].mPMIncoming.Retrieve();
				if (!ProcessPacket(frontPacket)) {
					CloseConnection(i, "Failed to process incoming packet. Closing connection");
					break;
				}
				mConnections[i].mPMIncoming.Pop();
			}
		}
	}

	void Server::OnConnect(TCPConnection & newConnection)
	{
		std::cout << newConnection.ToString() << " - New connection accepted\n";

	}

	void Server::OnDisconnect(TCPConnection & lostConnection, std::string reason)
	{
		std::cout << lostConnection.ToString() << " - connection lost\n";
	}

	void Server::CloseConnection(int connectionIndex, std::string reason)
	{
		TCPConnection &connection = mConnections[connectionIndex];
		std::cout << "[ " << reason << " ]" << connection.ToString() << std::endl;
		mMasterFDs.erase(mMasterFDs.begin() + connectionIndex + 1);
		mTempFDs.erase(mTempFDs.begin() + connectionIndex + 1);
		connection.Close();
		mConnections.erase(mConnections.begin() + connectionIndex);
	}

	bool Server::ProcessPacket(std::shared_ptr<Packet> p)
	{
		switch (p->GetPacketType()) {
		case IFTCP::PacketType::PT_ChatMessage:
		{
			std::string str;
			(*p) >> str;
			std::cout << "Chat Message : " << str;
			break;
		}
		case IFTCP::PacketType::PT_IntegerArray: {
			uint32_t sz = 0;
			(*p) >> sz;
			//sz = htonl(sz);
			for (int i = 0; i < sz; i++) {
				int val;
				(*p) >> val;
				std::cout << val << " , ";
			}
			std::cout << std::endl;
			break;
		}
		case IFTCP::PacketType::PT_FloatArray: {
			uint32_t sz = 0;
			(*p) >> sz;
			//sz = htonl(sz);
			for (int i = 0; i < sz; i++) {
				float val;
				(*p) >> val;
				std::cout << (float)val << " , ";
			}
			std::cout << std::endl;
			break;
		}
		case IFTCP::PacketType::PT_DoubleArray: {
			uint32_t sz = 0;
			(*p) >> sz;
			//sz = htonl(sz);
			for (int i = 0; i < sz; i++) {
				double val;
				(*p) >> val;
				std::cout << (double)val << " , ";
			}
			std::cout << std::endl;
			break;
		}
		case IFTCP::PacketType::PT_CharArray: {
			uint32_t sz = 0;
			(*p) >> sz;
			//sz = htonl(sz);
			for (int i = 0; i < sz; i++) {
				char val;
				(*p) >> val;
				std::cout << val << " , ";
			}
			std::cout << std::endl;
			break;
		}
		default:
			std::cout << "unknown packet type\n";
			return false;
		}
		return true;
	}

}