#include "IFTCP.h"
#include <iostream>
namespace IFTCP {
	int Foo(int i) {
		return i * 3;
	}

	bool Network::Initialise() {
		WSADATA wsadata;
		int result = WSAStartup(MAKEWORD(2, 2), &wsadata);
		//std::cout << int(LOBYTE(wsadata.wVersion)) << "," << int(HIBYTE(wsadata.wVersion)) << std::endl;
		if (result != 0) {
			std::cerr << "Failed to start Windows Sockets API\n";
			return false;
		}
		if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wVersion) != 2) {
			std::cerr << "Failed to get requested version of Windows Sockets API\n";
			return false;
		}
		return true;
	}

	void Network::Shutdown() {
		WSACleanup();
	}
}