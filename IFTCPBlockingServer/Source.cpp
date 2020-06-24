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
			//if (sock.Bind(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {//only local machine can connect
			//	std::cout << "socket successfully bound\n";
			//}
			if (sock.Listen(IFTCP::IPEndpoint("127.0.0.1", 9999),5) == IFTCP::PResult::P_Success) {//only local machine can connect
				std::cout << "socket successfully listening\n";
				IFTCP::Socket outSocket;
				if (sock.Accept(outSocket) == IFTCP::PResult::P_Success) {
					std::cout << "socket successfully accepted\n";	

					//char buffer[sizeof(uint32_t)];
					//strcpy_s(buffer, "Will finish this today\n");
					/*int bytesReceived = 0;
					int result = IFTCP::PResult::P_Success;
					std::string buffer = "";*/

					IFTCP::Packet p;
					//std::vector<int> vals;
					std::string str1;
					std::string str2;
					while (true) {
						IFTCP::PResult result = outSocket.Receive(p);
						if (result != IFTCP::PResult::P_Success) {
							break;
						}
						p >> str1;
						p >> str2;
						uint32_t val;
						p >> val;
						std::cout << str1;
						std::cout << str2;
						std::cout << (int)val << std::endl;
						//int sz = p.mBuffer.size() / sizeof(uint32_t);
						////vals.resize(sz);
						//for (int i = 0; i < sz; i++) {
						//	uint32_t val = 0;
						//	p >> val;
						//	//val = (ntohl(val));
						//	std::cout << val << std::endl;
						//}
						
						//uint32_t nBytes = 0;
						//result = outSocket.ReceiveAll(&nBytes, sizeof(uint32_t));
						//if (result != IFTCP::PResult::P_Success) {
						//	break;
						//}						
						//nBytes = ntohl(nBytes);
						////char *vals = new char[nBytes];

						//if (nBytes > IFTCP::gMaxPacketSize) {
						//	break;
						//}
						//buffer.resize(nBytes);

						//result = outSocket.ReceiveAll(&buffer[0], nBytes);

						//if (result != IFTCP::PResult::P_Success) {
						//	break;
						//}
						//std::cout << buffer << std::endl;
						//delete[] vals;
						
					}
				}
				else {
					std::cout << "Failed to accept new connection\n";
				}
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