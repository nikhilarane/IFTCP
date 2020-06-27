#ifndef _IPENDPOINT_
#define _IPENDPOINT_
#include "IPVersion.h"
#include <string>
#include <vector>
#include <WS2tcpip.h>

namespace IFTCP {
	class IPEndpoint {
	public:
		IPEndpoint() {}
		IPEndpoint(const char* ip, unsigned short port);
		IPEndpoint(sockaddr* addr);
		IPVersion GetIPVersion();
		std::vector<uint8_t> GetIPBytes();
		std::string GetHostname();
		std::string GetIPString();
		unsigned short GetPort();
		sockaddr_in GetSockaddrIPv4();
		sockaddr_in6 GetSockaddrIPv6();
		void Print();
	private:
		IPVersion mIPVersion = IPVersion::Unknown;
		std::string mHostname = "";
		std::string mIPString = "";
		unsigned short mPort = 0;
		std::vector<uint8_t> mIPBytes;

	};
}

#endif
