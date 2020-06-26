#ifndef _SERVER_
#define _SERVER_
#include <IFTCP\IncludeIFTCPIP.h>

using namespace IFTCP;

class Server {
public:
	bool Initialise(IPEndpoint endPoint);
	void Frame();
private:
	Socket mListeningSocket;
};

#endif
