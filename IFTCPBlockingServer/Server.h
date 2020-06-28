#ifndef _SERVER_
#define _SERVER_
#include <IFTCP\IncludeIFTCPIP.h>

using namespace IFTCP;

class Server {
public:
	bool Initialise(IPEndpoint endPoint);
	void Frame();
	void CloseConnection(int connectionIndex, std::string reason);
	bool ProcessPacket(Packet& packet);
private:
	Socket mListeningSocket;
	std::vector<TCPConnection> mConnections;
	std::vector<WSAPOLLFD> mMasterFDs;
	const int mAcceptTimeOut = 2;//2ms
	std::vector<WSAPOLLFD> mTempFDs;
};

#endif
