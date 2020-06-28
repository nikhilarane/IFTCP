#ifndef _SERVER_
#define _SERVER_
#include "TCPConnection.h"

namespace IFTCP {

	class Server {
	public:
		bool Initialise(IPEndpoint endPoint);
		void Frame();
	protected:
		virtual void OnConnect(TCPConnection & newConnection);
		virtual void OnDisconnect(TCPConnection& lostConnection, std::string reason);
		void CloseConnection(int connectionIndex, std::string reason);
		virtual bool ProcessPacket(std::shared_ptr<Packet> packet);	
		Socket mListeningSocket;
		std::vector<TCPConnection> mConnections;
		std::vector<WSAPOLLFD> mMasterFDs;
		const int mAcceptTimeOut = 2;//2ms
		std::vector<WSAPOLLFD> mTempFDs;
	};


}

#endif
