#ifndef _SERVER1_
#define _SERVER1_

#include <IFTCP\IncludeIFTCPIP.h>

using namespace IFTCP;

class Server1 : public Server {
private:
	void OnConnect(TCPConnection & newConnection) override;
	void OnDisconnect(TCPConnection& lostConnection, std::string reason) override;
	bool ProcessPacket(std::shared_ptr<Packet> packet) override;
};
#endif
