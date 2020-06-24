#ifndef _PACKET_
#define _PACKET_
#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <winsock.h>
#include <string>
#include "Constants.h"
#include "Packet.h"

namespace IFTCP {
	class Packet {
	public:
		void Clear();
		void Append(const void *data, uint32_t size);
		Packet& operator << (uint32_t data);
		Packet& operator >> (uint32_t& data);
		Packet& operator << (const std::string& data);
		Packet& operator >> (std::string& data);
		uint32_t mExtractionOffset = 0;
		std::vector<char> mBuffer;	
	};
}
#endif
