#ifndef _PACKETMANAGER_
#define _PACKETMANAGER_
#include <queue>
#include "Packet.h"
#include <memory>

namespace IFTCP {
	enum PacketManagerTask {
		ProcessPacketSize,
		ProcessPacketContents
	};
	class PacketManager {
	private:
		std::queue<std::shared_ptr<Packet>> mPackets;
	public:
		void Clear();
		bool HasPendingPackets();
		void Append(std::shared_ptr<Packet> packet);
		std::shared_ptr<Packet> Retrieve();
		void Pop();

		uint16_t mCurrentPacketSize = 0;
		int mCurrentPacketExtractionOffset = 0;
		PacketManagerTask mCurrentTask = PacketManagerTask::ProcessPacketSize;
	};
}

#endif
