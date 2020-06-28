#include "PacketManager.h"

namespace IFTCP {
	void PacketManager::Clear()
	{
		mPackets = std::queue<std::shared_ptr<Packet>>{};
	}

	bool PacketManager::HasPendingPackets()
	{
		return !(mPackets.empty());
	}

	void PacketManager::Append(std::shared_ptr<Packet> packet)
	{
		mPackets.push(std::move(packet));
		//moving a shared_ptr is faster than copying, because atomic increment/decrement operations are a lot slower than regular increment/decrement
		//move means the same shared_ptr is "stolen" rather than "copied" and the other one is nullified, thus avoiding atomic operations
		//By using move you avoid increasing, and then immediately decreasing, the number of shares
	}

	std::shared_ptr<Packet> PacketManager::Retrieve()
	{
		std::shared_ptr<Packet> p = mPackets.front();
		return p;
	}

	void IFTCP::PacketManager::Pop()
	{
		mPackets.pop();
	}


}
