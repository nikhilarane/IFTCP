#include <IFTCP\IncludeIFTCPIP.h>
#include <iostream>
int main() {
	//int val = IFTCP::Foo(10);
	//std::cout << val << std::endl;
	if (IFTCP::Network::Initialise()) {
		std::cout << "initialised\n";
		//IFTCP::IPEndpoint test("192.168.0.254", 8000);
		/*IFTCP::IPEndpoint test("www.google.com", 8000);
		if (test.GetIPVersion() == IFTCP::IPVersion::IPv4) {
			std::cout << "Host name : " << test.GetHostname() << std::endl;
			std::cout << "IP : " << test.GetIPString() << std::endl;
			std::cout << "Port : " << test.GetPort() << std::endl;
			std::cout << "IP Bytes : "  << std::endl;
			for (auto & digit : test.GetIPBytes()) {
				std::cout << (int)digit << std::endl;
			}
		}
		else {
			std::cout << "This is not an IP address\n";
		}*/
		IFTCP::Socket sock;

		if (sock.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			//sock.Bind(IFTCP::IPEndpoint("0.0.0.0", 9999));//wild card all machines can connect
			if (sock.Bind(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {//only local machine can connect
				std::cout << "socket successfully bound\n";
			}
			if (sock.Close() == IFTCP::PResult::P_Success) {
				std::cout << "Socket Succesfully Closed\n";
			}
		}
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}