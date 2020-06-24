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
				//std::cout << "Socket Succesfully connected\n";
				//char buffer[256];
				//strcpy_s(buffer, "Will finish this today\n");
				//int bytesSent = 0;
				//int result = IFTCP::PResult::P_Success;
				//std::string buffer = "I will finish this today\n";
				//std::string buffer = "";
				//buffer.resize(IFTCP::gMaxPacketSize + 1);
				//memset(&buffer[0], 'A', IFTCP::gMaxPacketSize + 1);
				
				IFTCP::Packet p;
				//uint32_t a(4), b(5), c(10);
				//p << a << b << c << str;
				std::string str1("Taking too long\n");
				std::string str2("Finish it fast\n");
				int a = -113;
				p << str1 << str2 << a;
				while (true) {
					IFTCP::PResult result = sock.Send(p);
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					std::cout << "Attempting to Send ..." << std::endl;
					Sleep(1000);
					/*uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize);
					int result = sock.SendAll(&bufferSize, sizeof(uint32_t));
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					result = sock.SendAll(buffer.data(), buffer.size());
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					std::cout << "Attempting to Send ..." << std::endl;
					Sleep(1000);*/
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