#ifndef _PACKET_
#define _PACKET_
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <winsock.h>
#include <string>
#include "Constants.h"
#include "PacketException.h"
#include "PacketType.h"

namespace IFTCP {
	class Packet {
	public:
		Packet(PacketType packetType = PacketType::PT_Invalid);
		PacketType GetPacketType();
		void AssignPacketType(PacketType packetType);
		void Clear();
		void Append(const void *data, uint32_t size);
		Packet& operator << (uint32_t data);
		Packet& operator >> (uint32_t& data);
		Packet& operator << (const std::string& data);
		Packet& operator >> (std::string& data);
		Packet& operator << (const float data);
		Packet& operator >> (float& data);
		Packet& operator << (const int data);
		Packet& operator >> (int& data);
		uint32_t mExtractionOffset = 0;
		std::vector<char> mBuffer;	
	};
}
#endif
