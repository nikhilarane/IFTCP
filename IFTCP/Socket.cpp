#include "Socket.h"
#include <assert.h>
#include <iostream>

namespace IFTCP {
	Socket::Socket(IPVersion ipVersion, SocketHandle socketHandle):mIPVersion(ipVersion),mSocketHandle(socketHandle)
	{
		assert(mIPVersion == IPVersion::IPv4);
	}
	PResult Socket::Create()
	{
		assert(mIPVersion == IPVersion::IPv4);

		if (mSocketHandle != INVALID_SOCKET) {
			return PResult::P_GenericError;
		}

		mSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSocketHandle == INVALID_SOCKET) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success) {
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
	PResult Socket::Close()
	{
		if (mSocketHandle == INVALID_SOCKET) {
			return PResult::P_GenericError;
		}

		int result = closesocket(mSocketHandle);
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		mSocketHandle = INVALID_SOCKET;
		return PResult();
	}
	PResult Socket::Bind(IPEndpoint endPoint)
	{
		sockaddr_in addr = endPoint.GetSockaddrIPv4();
		int result = bind(mSocketHandle,(sockaddr*) &addr, sizeof(sockaddr_in));
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
	PResult Socket::Listen(IPEndpoint endPoint, int backlog)
	{
		if (Bind(endPoint) != PResult::P_Success) {
			return PResult::P_GenericError;
		}
		int result = listen(mSocketHandle, backlog);
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
	SocketHandle Socket::GetSocketHandle()
	{
		return mSocketHandle;
	}
	IPVersion Socket::GetIPVersion()
	{
		return mIPVersion;
	}
	PResult Socket::SetSocketOption(SocketOption option, BOOL value)
	{
		int result = 0;
		switch (option) {
		case SocketOption::TCP_NoDelay:
			result = setsockopt(mSocketHandle, IPPROTO_TCP, TCP_NODELAY, (const char *)&value, sizeof(value));
			break;
		default:
			return PResult::P_GenericError;
		}
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
	PResult Socket::Accept(Socket & outSocket)
	{
		sockaddr_in addr = {};
		int len = sizeof(sockaddr_in);
		SocketHandle acceptedConnectionHandle = accept(mSocketHandle, (sockaddr*)&addr, &len);
		if (acceptedConnectionHandle == INVALID_SOCKET) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		std::cout << "New Connection Accepted!\n";
		IPEndpoint newConnectionEndPoint((sockaddr*)&addr);
		newConnectionEndPoint.Print();
		outSocket = Socket(IPVersion::IPv4, acceptedConnectionHandle);

		return PResult::P_Success;
	}
	PResult Socket::Connect(IPEndpoint endPoint)
	{
		sockaddr_in addr = endPoint.GetSockaddrIPv4();
		int result = connect(mSocketHandle, (sockaddr*)(&addr), sizeof(sockaddr_in));
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(const void* data, int numberOfBytes, int & bytesSent)
	{
		bytesSent = send(mSocketHandle, (const char*)data, numberOfBytes, NULL);
		if (bytesSent == SOCKET_ERROR) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::Receive(void * destination, int numberOfBytes, int & bytesReceived)
	{
		bytesReceived = recv(mSocketHandle, (char*)destination, numberOfBytes, NULL);
		if (bytesReceived == 0) {//connection gracefuly closed
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}

	PResult Socket::SendAll(const void * data, int numberOfBytes)
	{
		int totalBytesSent = 0;
		while (totalBytesSent < numberOfBytes) {
			int bytesToSend = numberOfBytes - totalBytesSent;
			char *offset = (char *)data + totalBytesSent;
			int bytesSent = 0;
			int result = Send(offset, bytesToSend, bytesSent);
			if (result != PResult::P_Success) {
				return PResult::P_GenericError;
			}
			totalBytesSent += bytesSent;
		}
		return PResult::P_Success;
	}

	PResult Socket::ReceiveAll(void * destination, int numberOfBytes)
	{
		int totalBytesReceived = 0;
		while (totalBytesReceived < numberOfBytes) {
			int bytesToReceive = numberOfBytes - totalBytesReceived;
			char *offset = (char *)destination + totalBytesReceived;
			int bytesReceived = 0;
			int result = Receive(offset, bytesToReceive, bytesReceived);
			if (result != PResult::P_Success) {
				return PResult::P_GenericError;
			}
			totalBytesReceived += bytesReceived;
		}
		return PResult::P_Success;
	}

	PResult Socket::Send(Packet & packet)
	{
		uint32_t encodedPacketSize = htonl(packet.mBuffer.size());
		PResult result = SendAll(&encodedPacketSize, sizeof(uint32_t));
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		result = SendAll(packet.mBuffer.data(), packet.mBuffer.size());
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		return PResult::P_Success;
	}

	PResult Socket::Receive(Packet & packet)
	{
		packet.Clear();
		uint32_t encodedPacketSize = 0;// = ntohl(sizeof(uint32_t));
		PResult result = ReceiveAll(&encodedPacketSize, sizeof(uint32_t));
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		encodedPacketSize = ntohl(encodedPacketSize);
		if (encodedPacketSize > gMaxPacketSize) {
			return PResult::P_GenericError;
		}
		packet.mBuffer.resize(encodedPacketSize);
		result = ReceiveAll(&packet.mBuffer[0], encodedPacketSize);
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		return PResult::P_Success;
	}
	
}