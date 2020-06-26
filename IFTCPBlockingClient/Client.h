#ifndef _CLIENT_
#define _CLIENT_
#include <IFTCP\IncludeIFTCPIP.h>

using namespace IFTCP;

class Client {
public:
	bool Connect(IPEndpoint endPoint);
	bool IsConnected();
	bool Frame();
private:
	bool mIsConnected;
	Socket mSocket;
};
#endif
