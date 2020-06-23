#include "IPEndpoint.h"
#include <iostream>
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

	IPEndpoint::IPEndpoint(sockaddr* addr){
		assert(addr->sa_family = AF_INET);
		sockaddr_in* addrv4 = reinterpret_cast<sockaddr_in*>(addr);
		mIPVersion = IPVersion::IPv4;
		mPort = ntohs(addrv4->sin_port);
		mIPBytes.resize(sizeof(ULONG));
		memcpy_s(&mIPBytes[0], sizeof(ULONG), &addrv4->sin_addr, sizeof(ULONG));
		mIPString.resize(16);
		inet_ntop(AF_INET, &addrv4->sin_addr, &mIPString[0], 16);
		mHostname = mIPString;

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

	void IPEndpoint::Print()
	{
		switch (mIPVersion) {
		case IPVersion::IPv4:
			std::cout << "Internet Protocol : IPv4 " << std::endl;
			break;
		case IPVersion::IPv6:
			std::cout << "Internet Protocol : IPv6 " << std::endl;
			break;
		default:
			std::cout << "Internet Protocol : unknown " << std::endl;
			break;

		}
		std::cout << "Host name : " << GetHostname() << std::endl;
		std::cout << "IP : " << GetIPString() << std::endl;
		std::cout << "Port : " << GetPort() << std::endl;
		std::cout << "IP Bytes : " << std::endl;
		for (auto & digit : GetIPBytes()) {
			std::cout << (int)digit << std::endl;
		}
	}

}

