#include "Client1.h"
#include <iostream>

void Client1::OnConnect()
{
	std::cout << " Connected to the server\n";
	std::shared_ptr<Packet> welcomeMessage = std::make_shared<Packet>(PacketType::PT_ChatMessage);
	*welcomeMessage << "Hello from the client " + mConnection.ToString() + "\n";
	mConnection.mPMOutGoing.Append(welcomeMessage);

}

bool Client1::ProcessPacket(std::shared_ptr<Packet> p)
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
		return true;
}

