#include "Client.h"
#include <iostream>

namespace IFTCP {
	bool Client::Connect(IPEndpoint endPoint)
	{
		mIsConnected = false;
		//if (Network::Initialise()) {
		//	std::cout << "initialised\n";
			//IFTCP::Socket sock;
			Socket socket = IFTCP::Socket(endPoint.GetIPVersion());
			if (socket.Create() == IFTCP::PResult::P_Success) {
				
				if (socket.SetBlocking(true) != PResult::P_Success) {
					//return PResult::P_GenericError;
					return false;
				}
				std::cout << "Socket Succesfully Created\n";
				//if (sock.Connect(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {
				if (socket.Connect(endPoint) == IFTCP::PResult::P_Success) {
					if (socket.SetBlocking(false) == PResult::P_Success) {
						mConnection = TCPConnection(socket, endPoint);
						mMasterFD.fd = mConnection.mSocket.GetSocketHandle();
						//mMasterFD.events = POLLRDNORM | POLLWRNORM;
						mMasterFD.events = POLLRDNORM;
						mMasterFD.revents = 0;
						mIsConnected = true;
						OnConnect();
						return true;
					}
					
				}
				else {
					//std::cout << "Socket connection failed\n";
				}
				socket.Close();
			}
			else {
				std::cout << "Failed to create socket\n";
			}
		//}
			OnConnectFail();
		return false;
	}

	bool Client::IsConnected()
	{
		return mIsConnected;
	}

	bool Client::Frame()
	{

		//IFTCP::Packet stringPacket(IFTCP::PacketType::PT_ChatMessage);
		//stringPacket << "Hello TCP World\n";

		//IFTCP::Packet integerArrayPacket(IFTCP::PacketType::PT_IntegerArray);
		//const uint32_t array_size = 6;
		//int integerArray[array_size] = { -1,-5,0,18,100,52 };
		//integerArrayPacket << array_size;
		//for (auto var : integerArray)
		//{
		//	integerArrayPacket << var;
		//}
		//IFTCP::Packet floatArrayPacket(IFTCP::PacketType::PT_FloatArray);
		//const uint32_t floatarray_size = 5;
		//float floatintegerArray[floatarray_size] = { -1.8f,-5.3f,0.0f,18.99f,100.897f };
		//floatArrayPacket << floatarray_size;
		//for (auto var : floatintegerArray)
		//{
		//	floatArrayPacket << var;
		//}

		//IFTCP::Packet doubleArrayPacket(IFTCP::PacketType::PT_DoubleArray);
		//const uint32_t doublearray_size = 7;
		//double doubleintegerArray[doublearray_size] = { 10.4,-1.8,-5.3,0.0,18.99, 100.897, 6789564.87654 };
		//doubleArrayPacket << doublearray_size;
		//for (auto var : doubleintegerArray)
		//{
		//	doubleArrayPacket << var;
		//}

		//IFTCP::Packet charArrayPacket(IFTCP::PacketType::PT_CharArray);
		//const uint32_t chararray_size = 2;
		//char charArray[chararray_size] = { 'N','R' };
		//charArrayPacket << chararray_size;
		//for (auto var : charArray)
		//{
		//	charArrayPacket << var;
		//}
		//
		////IFTCP::PResult result = sock.Send(p);
		//IFTCP::PResult result;
		//int gamble = rand() % 5;
		//if (gamble == 0) {
		//	result = mSocket.Send(integerArrayPacket);
		//}
		//else if (gamble == 1) {

		//	result = mSocket.Send(stringPacket);
		//}
		//else if (gamble == 2) {

		//	result = mSocket.Send(doubleArrayPacket);
		//}
		//else if (gamble == 3) {

		//	result = mSocket.Send(charArrayPacket);
		//}
		//else {
		//	result = mSocket.Send(floatArrayPacket);
		//}
		//if (result != IFTCP::PResult::P_Success) {
		//	mIsConnected = false;
		//	return false;
		//}
		//std::cout << "Attempting to Send ..." << std::endl;
		//Sleep(2000);
		//return true;

		/*Packet incomingPacket;
		if (mSocket.Receive(incomingPacket) != PResult::P_Success) {
			std::cout << "Lost connection?\n";
			mIsConnected = false;
			return false;
		}
		if (!ProcessPacket(incomingPacket)) {
			std::cout << "Could not process Packet\n";
			mIsConnected = false;
			return false;
		}*/
		if (mConnection.mPMOutGoing.HasPendingPackets()) {
			mMasterFD.events = POLLRDNORM | POLLWRNORM;
		}

		mTempFD = mMasterFD;
		if (WSAPoll(&mTempFD, 1, mAcceptTimeOut) > 0) {
			//for (int i = mTempFDs.size() - 1; i > 0; i--) {
				
				if (mTempFD.revents & POLLERR) {
					CloseConnection("Poll error occured on ");
					return false;
				}

				if (mTempFD.revents & POLLHUP) {
					CloseConnection("Poll hang up occured on ");
					return false;
				}

				if (mTempFD.revents & POLLNVAL) {
					CloseConnection("Poll invalid socket occured on ");
					return false;
				}

				if (mTempFD.revents & POLLRDNORM) {
					int bytesReceived = 0;
					if (mConnection.mPMIncoming.mCurrentTask == PacketManagerTask::ProcessPacketSize) {

						bytesReceived = recv(mTempFD.fd, (char*)&mConnection.mPMIncoming.mCurrentPacketSize + mConnection.mPMIncoming.mCurrentPacketExtractionOffset, sizeof(uint16_t) - mConnection.mPMIncoming.mCurrentPacketExtractionOffset, 0);


					}
					else {
						bytesReceived = recv(mTempFD.fd, (char*)&mConnection.mBuffer + mConnection.mPMIncoming.mCurrentPacketExtractionOffset, mConnection.mPMIncoming.mCurrentPacketSize - mConnection.mPMIncoming.mCurrentPacketExtractionOffset, 0);
					}


					//
					if (bytesReceived == 0) {
						CloseConnection("[Recv == 0] Connection was lost on ");

					}

					if (bytesReceived == SOCKET_ERROR) {
						int error = WSAGetLastError();
						if (error != WSAEWOULDBLOCK) {
							CloseConnection("[Recv < 0] Connection was lost on");

						}
					}

					if (bytesReceived > 0) {
						mConnection.mPMIncoming.mCurrentPacketExtractionOffset += bytesReceived;
						if (mConnection.mPMIncoming.mCurrentTask == PacketManagerTask::ProcessPacketSize) {
							if (mConnection.mPMIncoming.mCurrentPacketExtractionOffset == sizeof(uint16_t)) {
								mConnection.mPMIncoming.mCurrentPacketSize = ntohs(mConnection.mPMIncoming.mCurrentPacketSize);
								if (mConnection.mPMIncoming.mCurrentPacketSize > IFTCP::gMaxPacketSize) {
									CloseConnection("Packet size too large. Closing connection");
									return false;
								}
								mConnection.mPMIncoming.mCurrentPacketExtractionOffset = 0;
								mConnection.mPMIncoming.mCurrentTask = PacketManagerTask::ProcessPacketContents;
							}
						}
						else {
							if (mConnection.mPMIncoming.mCurrentPacketExtractionOffset == mConnection.mPMIncoming.mCurrentPacketSize) {
								//Packet p;
								//p.mBuffer.resize(connection.mPMIncoming.mCurrentPacketSize);
								//memcpy_s(&p.mBuffer[0], connection.mPMIncoming.mCurrentPacketSize, &connection.mBuffer[0], connection.mPMIncoming.mCurrentPacketSize);
								//std::cout << connection.ToString() << " : ";
								/*if (!ProcessPacket(p)) {
								CloseConnection(connectionIndex, "Failed to process Packet. Closing connection");
								continue;
								}*/
								std::shared_ptr<Packet> p = std::make_shared<Packet>();
								p->mBuffer.resize(mConnection.mPMIncoming.mCurrentPacketSize);
								memcpy_s(&(p->mBuffer[0]), mConnection.mPMIncoming.mCurrentPacketSize, mConnection.mBuffer, mConnection.mPMIncoming.mCurrentPacketSize);
								mConnection.mPMIncoming.Append(p);
								//std::cout << connection.ToString() << " : ";

								mConnection.mPMIncoming.mCurrentPacketSize = 0;
								mConnection.mPMIncoming.mCurrentPacketExtractionOffset = 0;
								mConnection.mPMIncoming.mCurrentTask = PacketManagerTask::ProcessPacketSize;
							}
						}
						//std::cout << " Message of size " << bytesReceived << " received from " << connection.ToString() << std::endl;
					}
				}

				if (mTempFD.revents & POLLWRNORM) {
					PacketManager& pm = mConnection.mPMOutGoing;

					while (pm.HasPendingPackets()) {
						if (pm.mCurrentTask == PacketManagerTask::ProcessPacketSize) {
							pm.mCurrentPacketSize = pm.Retrieve()->mBuffer.size();
							uint16_t bigEndianPacketSize = htons(pm.mCurrentPacketSize);
							int bytesSent = send(mTempFD.fd, (char*)&bigEndianPacketSize + pm.mCurrentPacketExtractionOffset, sizeof(uint16_t) - pm.mCurrentPacketExtractionOffset, 0);
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
							int bytesSent = send(mTempFD.fd, (char*)bufferPtr + pm.mCurrentPacketExtractionOffset, pm.mCurrentPacketSize - pm.mCurrentPacketExtractionOffset, 0);
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
					if (!mConnection.mPMOutGoing.HasPendingPackets()) {
						mMasterFD.events = POLLRDNORM;
					}
				}
			//}

			
			
		}
		//for (int i = mConnections.size() - 1; i >= 0; i--) {
			while (mConnection.mPMIncoming.HasPendingPackets()) {
				std::shared_ptr<Packet> frontPacket = mConnection.mPMIncoming.Retrieve();
				if (!ProcessPacket(frontPacket)) {
					CloseConnection("Failed to process incoming packet. Closing connection");
					return false;
				}
				mConnection.mPMIncoming.Pop();
			}
			
		//}
		//connected sockets
		
		return true;
	}

	bool Client::SendPacket(Packet p)
	{
		std::shared_ptr<Packet> message = std::make_shared<Packet>(p);
		if (IsConnected()) {
			mConnection.mPMOutGoing.Append(message);
			return true;
		}
		return false;
	}

	void Client::OnConnect()
	{
		std::cout << "Successfully connected\n";
	}

	void Client::OnConnectFail()
	{
		std::cout << "Failed to Connect\n";
	}

	void Client::OnDisconnect(std::string reason)
	{
		std::cout << "Lost connection due to " << reason << std::endl;
	}

	bool Client::ProcessPacket(std::shared_ptr<Packet> p)
	{
		std::cout << "Received packet of size " << p->mBuffer.size() << std::endl;
		return true;
	}

	void Client::CloseConnection(std::string reason)
	{
		OnDisconnect(reason);
		mMasterFD.fd = 0;
		mIsConnected = false;
		mConnection.Close();
		
	}

	//bool Client::ProcessPacket(Packet & p)
	//{
	//	switch (p.GetPacketType()) {
	//	case IFTCP::PacketType::PT_ChatMessage:
	//	{
	//		std::string str;
	//		p >> str;
	//		std::cout << "Chat Message : " << str;
	//		break;
	//	}
	//	case IFTCP::PacketType::PT_IntegerArray: {
	//		uint32_t sz = 0;
	//		p >> sz;
	//		//sz = htonl(sz);
	//		for (int i = 0; i < sz; i++) {
	//			int val;
	//			p >> val;
	//			std::cout << val << " , ";
	//		}
	//		std::cout << std::endl;
	//		break;
	//	}
	//	case IFTCP::PacketType::PT_FloatArray: {
	//		uint32_t sz = 0;
	//		p >> sz;
	//		//sz = htonl(sz);
	//		for (int i = 0; i < sz; i++) {
	//			float val;
	//			p >> val;
	//			std::cout << (float)val << " , ";
	//		}
	//		std::cout << std::endl;
	//		break;
	//	}
	//	case IFTCP::PacketType::PT_DoubleArray: {
	//		uint32_t sz = 0;
	//		p >> sz;
	//		//sz = htonl(sz);
	//		for (int i = 0; i < sz; i++) {
	//			double val;
	//			p >> val;
	//			std::cout << (double)val << " , ";
	//		}
	//		std::cout << std::endl;
	//		break;
	//	}
	//	case IFTCP::PacketType::PT_CharArray: {
	//		uint32_t sz = 0;
	//		p >> sz;
	//		//sz = htonl(sz);
	//		for (int i = 0; i < sz; i++) {
	//			char val;
	//			p >> val;
	//			std::cout << val << " , ";
	//		}
	//		std::cout << std::endl;
	//		break;
	//	}
	//	default:
	//		std::cout << "unknown packet type\n";
	//		return false;
	//	}
	//	return true;
	//}

}