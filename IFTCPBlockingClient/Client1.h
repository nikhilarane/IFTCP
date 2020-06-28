#ifndef _CLIENT1_
#define _CLIENT1_
#include <IFTCP\IncludeIFTCPIP.h>

using namespace IFTCP;

class Client1 : public Client {
	void OnConnect()override;
	//void OnConnectFail()override;
	//void OnDisconnect(std::string reason)override;
	bool ProcessPacket(std::shared_ptr<Packet> p)override;
};

#endif
