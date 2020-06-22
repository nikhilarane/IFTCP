#include <IFTCP\IFTCP.h>
#include <iostream>
int main() {
	//int val = IFTCP::Foo(10);
	//std::cout << val << std::endl;
	if (IFTCP::Network::Initialise()) {
		std::cout << "initialised\n";
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}