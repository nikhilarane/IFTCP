#ifndef _TCPCONNECTION_
#define _TCPCONNECTION_
#include "Socket.h"

namespace IFTCP {
	enum PacketTask {
		ProcessPacketSize,
		ProcessPacketContents
	};
	class TCPConnection {
	public:
		
		TCPConnection(Socket socket, IPEndpoint endPoint);
		void Close();
		std::string ToString();
		Socket mSocket;
		PacketTask mPacketTask = PacketTask::ProcessPacketSize;
		int mExtractionOffset = 0;
		char mBuffer[IFTCP::gMaxPacketSize];
		uint16_t mPacketSize;
	private:
		
		IPEndpoint mEndPoint;
		std::string mStringRepresentation = "";

	};
}
#endif
