#include <IFTCP\IncludeIFTCPIP.h>
#include <iostream>
int main() {
	//int val = IFTCP::Foo(10);
	//std::cout << val << std::endl;
	if (IFTCP::Network::Initialise()) {
		std::cout << "initialised\n";
		IFTCP::Socket sock;
		if (sock.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			if (sock.Connect(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {
				std::cout << "Socket Succesfully connected\n";
				char buffer[256];
				strcpy_s(buffer, "Will finish this today\n");
				int bytesSent = 0;
				int result = IFTCP::PResult::P_Success;
				while (result == IFTCP::PResult::P_Success) {
					result = sock.SendAll(buffer, 256);
					std::cout << "Attempting to Send ..." << std::endl;
					Sleep(500);
				}
			}
			else {
				std::cout << "Socket connection failed\n";
			}
			sock.Close();
		}
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}