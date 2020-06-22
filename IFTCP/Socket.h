#ifndef _SOCKET_
#define _SOCKET_
#include "SocketHandle.h"
#include "PResult.h"
#include "IPVersion.h"
#include "SocketOption.h"

namespace IFTCP {
	class Socket {
	public:
		Socket(IPVersion ipVersion = IPVersion::IPv4, SocketHandle socketHandle = INVALID_SOCKET);
		PResult Create();
		PResult Close();
		SocketHandle GetSocketHandle();
		IPVersion GetIPVersion();
		PResult SetSocketOption(SocketOption option, BOOL value);
	private:
		
		IPVersion mIPVersion = IPVersion::IPv4;
		SocketHandle mSocketHandle = INVALID_SOCKET;
		int mWSAErrorCode;
	};
}

#endif
