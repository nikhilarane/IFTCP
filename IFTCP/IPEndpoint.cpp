#include "IPEndpoint.h"

#include <assert.h>

namespace IFTCP {
	IPEndpoint::IPEndpoint(const char* ip, unsigned short port)
	{
		this->mPort = port;
		in_addr addr;//a union of 4 bytes to store ipv4 address
		int result = inet_pton(AF_INET, ip, &addr);
		if (result == 1) {
			if (addr.S_un.S_addr != INADDR_NONE) {
				mHostname = ip;
				mIPString = ip;
				mIPBytes.resize(sizeof(ULONG));
				memcpy_s(&mIPBytes[0], sizeof(ULONG), &addr.S_un.S_addr, sizeof(ULONG));
				mIPVersion = IPVersion::IPv4;
				return;
			}
		}
		addrinfo hints = {};
		hints.ai_family = AF_INET;
		addrinfo* hostinfo = nullptr;
		result = getaddrinfo(ip, NULL, &hints, &hostinfo);
		if (result == 0) {
			sockaddr_in* host_addr = reinterpret_cast<sockaddr_in*>(hostinfo->ai_addr);
			mIPString.resize(16);
			inet_ntop(AF_INET, &host_addr->sin_addr, &mIPString[0], 16);
			mHostname = ip;
			ULONG ip_long = host_addr->sin_addr.S_un.S_addr;
			mIPBytes.resize(sizeof(ULONG));			
			memcpy_s(&mIPBytes[0], sizeof(ULONG), &ip_long, sizeof(ULONG));
			mIPVersion = IPVersion::IPv4;
			//host_addr->sin_addr.S_un.S_addr
			freeaddrinfo(hostinfo);
			return;
		}
	}

	IPVersion IPEndpoint::GetIPVersion()
	{
		return mIPVersion;
	}

	std::vector<uint8_t> IPEndpoint::GetIPBytes()
	{
		return mIPBytes;
	}

	std::string IPEndpoint::GetHostname()
	{
		return mHostname;
	}

	std::string IPEndpoint::GetIPString()
	{
		return mIPString;
	}

	unsigned short IPEndpoint::GetPort()
	{
		return mPort;
	}

	sockaddr_in IPEndpoint::GetSockaddrIPv4()
	{
		assert(mIPVersion == IPVersion::IPv4);
		sockaddr_in addr = {};
		memcpy_s(&addr.sin_addr, sizeof(ULONG), &mIPBytes[0], sizeof(ULONG));
		addr.sin_port = htons(mPort);
		addr.sin_family = AF_INET;
		return addr;
	}

}

