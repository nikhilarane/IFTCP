#include "Server1.h"
#include <iostream>

void Server1::OnConnect(TCPConnection & newConnection)
{
	std::cout << newConnection.ToString() << " - New connection accepted\n";
	std::shared_ptr<Packet> welcomeMessage = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*welcomeMessage << "Welcome to My Server " + newConnection.ToString() + "\n";
	newConnection.mPMOutGoing.Append(welcomeMessage);

	std::shared_ptr<Packet> p = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*p << "We just added a connection to " + newConnection.ToString() + "\n";

	for (auto& connection : mConnections) {
		if (&connection == &newConnection)
			continue;
		connection.mPMOutGoing.Append(p);
	}
}

void Server1::OnDisconnect(TCPConnection & lostConnection, std::string reason)
{
	std::cout << lostConnection.ToString() << " - connection lost\n";

	std::shared_ptr<Packet> p = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*p << "We just lost connection to " + lostConnection.ToString() + "\n";

	for (auto& connection : mConnections) {
		if (&connection == &lostConnection)
			continue;
		connection.mPMOutGoing.Append(p);
	}
}

bool Server1::ProcessPacket(std::shared_ptr<Packet> p)
{
	switch (p->GetPacketType()) {
	case IFTCP::PacketType::PT_ChatMessage:
	{
		std::string str;
		(*p) >> str;
		std::cout << "Chat Message : " << str;
		break;
	}
	case IFTCP::PacketType::PT_IntegerArray: {
		uint32_t sz = 0;
		(*p) >> sz;
		//sz = htonl(sz);
		for (int i = 0; i < sz; i++) {
			int val;
			(*p) >> val;
			std::cout << val << " , ";
		}
		std::cout << std::endl;
		break;
	}
	case IFTCP::PacketType::PT_FloatArray: {
		uint32_t sz = 0;
		(*p) >> sz;
		//sz = htonl(sz);
		for (int i = 0; i < sz; i++) {
			float val;
			(*p) >> val;
			std::cout << (float)val << " , ";
		}
		std::cout << std::endl;
		break;
	}
	case IFTCP::PacketType::PT_DoubleArray: {
		uint32_t sz = 0;
		(*p) >> sz;
		//sz = htonl(sz);
		for (int i = 0; i < sz; i++) {
			double val;
			(*p) >> val;
			std::cout << (double)val << " , ";
		}
		std::cout << std::endl;
		break;
	}
	case IFTCP::PacketType::PT_CharArray: {
		uint32_t sz = 0;
		(*p) >> sz;
		//sz = htonl(sz);
		for (int i = 0; i < sz; i++) {
			char val;
			(*p) >> val;
			std::cout << val << " , ";
		}
		std::cout << std::endl;
		break;
	}
	default:
		std::cout << "unknown packet type\n";
		return false;
	}
}
