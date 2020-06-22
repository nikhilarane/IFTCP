#include "Socket.h"
#include <assert.h>

namespace IFTCP {
	Socket::Socket(IPVersion ipVersion, SocketHandle socketHandle):mIPVersion(ipVersion),mSocketHandle(socketHandle)
	{
		assert(mIPVersion == IPVersion::IPv4);
	}
	PResult Socket::Create()
	{
		assert(mIPVersion == IPVersion::IPv4);

		if (mSocketHandle != INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		mSocketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (mSocketHandle == INVALID_SOCKET) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		if (SetSocketOption(SocketOption::TCP_NoDelay, TRUE) != PResult::P_Success) {
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}
	PResult Socket::Close()
	{
		if (mSocketHandle == INVALID_SOCKET) {
			return PResult::P_NotYetImplemented;
		}

		int result = closesocket(mSocketHandle);
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		mSocketHandle = INVALID_SOCKET;
		return PResult();
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
			return PResult::P_NotYetImplemented;
		}
		if (result != 0) {
			mWSAErrorCode = WSAGetLastError();
			return PResult::P_NotYetImplemented;
		}
		return PResult::P_Success;
	}
}