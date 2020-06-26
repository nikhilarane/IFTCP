#include "Socket.h"
#include <assert.h>
#include <iostream>

namespace IFTCP {
	Socket::Socket(IPVersion ipVersion, SocketHandle socketHandle):mIPVersion(ipVersion),mSocketHandle(socketHandle)
	{
		assert(mIPVersion == IPVersion::IPv4 || mIPVersion == IPVersion::IPv6);
	}
	PResult Socket::Create()
	{
		assert(mIPVersion == IPVersion::IPv4 || mIPVersion == IPVersion::IPv6);

		if (mSocketHandle != INVALID_SOCKET) {
			return PResult::P_GenericError;
		}

		mSocketHandle = socket((mIPVersion == IPVersion::IPv4) ? AF_INET : AF_INET6, SOCK_STREAM, IPPROTO_TCP);
		if (mSocketHandle == INVALID_SOCKET) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}

		if (SetBlocking(false) != PResult::P_Success) {
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
		assert(mIPVersion == endPoint.GetIPVersion());
		if (mIPVersion == IPVersion::IPv4) {
			sockaddr_in addr = endPoint.GetSockaddrIPv4();
			int result = bind(mSocketHandle, (sockaddr*)&addr, sizeof(sockaddr_in));
			if (result != 0) {
				mWSAErrorCode = WSAGetLastError();
				return PResult::P_GenericError;
			}
		}
		else {
			sockaddr_in6 addr = endPoint.GetSockaddrIPv6();
			int result = bind(mSocketHandle, (sockaddr*)&addr, sizeof(sockaddr_in6));
			if (result != 0) {
				mWSAErrorCode = WSAGetLastError();
				return PResult::P_GenericError;
			}
		}
		return PResult::P_Success;
	}
	PResult Socket::Listen(IPEndpoint endPoint, int backlog)
	{

		if (mIPVersion == IPVersion::IPv6) {
			if (SetSocketOption(SocketOption::IPV6_Only, false) != PResult::P_Success) {
				return PResult::P_GenericError;
			}
		}
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
		case SocketOption::IPV6_Only:
			result = setsockopt(mSocketHandle, IPPROTO_IPV6, IPV6_V6ONLY, (const char *)&value, sizeof(value));
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
		assert(mIPVersion == IPVersion::IPv4 || mIPVersion == IPVersion::IPv6);

		if (mIPVersion == IPVersion::IPv4) {
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

		}
		else {
			sockaddr_in6 addr = {};
			int len = sizeof(sockaddr_in6);
			SocketHandle acceptedConnectionHandle = accept(mSocketHandle, (sockaddr*)&addr, &len);
			if (acceptedConnectionHandle == INVALID_SOCKET) {
				mWSAErrorCode = WSAGetLastError();
				return PResult::P_GenericError;
			}
			std::cout << "New IPv6 Connection Accepted!\n";
			IPEndpoint newConnectionEndPoint((sockaddr*)&addr);
			newConnectionEndPoint.Print();
			outSocket = Socket(IPVersion::IPv6, acceptedConnectionHandle);

		}
		
		return PResult::P_Success;
	}
	PResult Socket::Connect(IPEndpoint endPoint)
	{
		assert(mIPVersion == endPoint.GetIPVersion());
		int result = 0;
		if (mIPVersion == IPVersion::IPv4) {
			sockaddr_in addr = endPoint.GetSockaddrIPv4();
			result = connect(mSocketHandle, (sockaddr*)(&addr), sizeof(sockaddr_in));
			
		}
		else {
			sockaddr_in6 addr = endPoint.GetSockaddrIPv6();
			result = connect(mSocketHandle, (sockaddr*)(&addr), sizeof(sockaddr_in6));
			
		}
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
		uint16_t encodedPacketSize = htons(packet.mBuffer.size());
		PResult result = SendAll(&encodedPacketSize, sizeof(uint16_t));
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
		uint16_t encodedPacketSize = 0;// = ntohl(sizeof(uint32_t));
		PResult result = ReceiveAll(&encodedPacketSize, sizeof(uint16_t));
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		encodedPacketSize = ntohs(encodedPacketSize);
		if (encodedPacketSize > gMaxPacketSize) {
			return PResult::P_GenericError;
		}
		packet.mBuffer.resize(encodedPacketSize);
		result = ReceiveAll(&packet.mBuffer[0], encodedPacketSize);
		if (result == PResult::P_GenericError)
			return PResult::P_GenericError;
		return PResult::P_Success;
	}

	PResult Socket::SetBlocking(bool isBlocking)
	{
		unsigned long nonBlocking = 1;
		unsigned long blocking = 0;
		int result = ioctlsocket(mSocketHandle, FIONBIO, isBlocking ? &blocking : &nonBlocking);
		if (result == SOCKET_ERROR) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_GenericError;
		}
		return PResult::P_Success;
	}
	
}