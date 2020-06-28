#ifndef _PACKETTYPE_
#define _PACKETTYPE_
#include <stdint.h>

namespace IFTCP {
	enum PacketType : uint16_t{
		PT_Invalid,
		PT_ChatMessage,
		PT_IntegerArray,
		PT_FloatArray,
		PT_DoubleArray,
		PT_CharArray,
		PT_Test
	};
}
#endif
