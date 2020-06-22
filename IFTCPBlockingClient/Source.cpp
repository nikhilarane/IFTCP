#include <IFTCP\IncludeIFTCPIP.h>
#include <iostream>
int main() {
	//int val = IFTCP::Foo(10);
	//std::cout << val << std::endl;
	if (IFTCP::Network::Initialise()) {
		std::cout << "initialised\n";
		IFTCP::Socket sock;
		if (sock.Create()) {
			std::cout << "Socket Succesfully Created\n";
			sock.Close();
		}
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}