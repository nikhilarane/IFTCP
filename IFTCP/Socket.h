#ifndef _SOCKET_
#define _SOCKET_
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"
#include "IPEndpoint.h"
#include "Constants.h"
#include "Packet.h"

namespace IFTCP {
	class Socket {
	public:
		Socket(IPVersion ipVersion = IPVersion::IPv4, SocketHandle socketHandle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		PResult Bind(IPEndpoint endPoint);
		PResult Listen(IPEndpoint endPoint, int backlog);
		SocketHandle GetSocketHandle();
		IPVersion GetIPVersion();
		PResult SetSocketOption(SocketOption option, BOOL value);
		PResult Accept(Socket& outSocket, IPEndpoint* endPoint = nullptr);
		PResult Connect(IPEndpoint endPoint);
		PResult Send(const void* data, int numberOfBytes, int& bytesSent);
		PResult Receive(void* destination, int numberOfBytes, int& bytesReceived);
		PResult SendAll(const void* data, int numberOfBytes);
		PResult ReceiveAll(void* destination, int numberOfBytes);
		PResult Send(Packet& packet);
		PResult Receive(Packet& packet);
		PResult SetBlocking(bool isBlocking);
	private:
		
		IPVersion mIPVersion = IPVersion::IPv4;
		SocketHandle mSocketHandle = INVALID_SOCKET;
		int mWSAErrorCode;
	};
}

#endif
