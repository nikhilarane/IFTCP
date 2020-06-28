#ifndef _CLIENT_
#define _CLIENT_
#include "TCPConnection.h"

namespace IFTCP {
	class Client {
	public:
		bool Connect(IPEndpoint endPoint);
		bool IsConnected();
		bool Frame();
	protected:
		virtual void OnConnect();
		virtual void OnConnectFail();
		virtual void OnDisconnect(std::string reason);
		virtual bool ProcessPacket(std::shared_ptr<Packet> p);
		TCPConnection mConnection;
		void CloseConnection(std::string reason);
	private:
		bool mIsConnected;
		WSAPOLLFD mMasterFD;
		WSAPOLLFD mTempFD;
		const int mAcceptTimeOut = 2;
		//Socket mSocket;
	};

}
#endif
