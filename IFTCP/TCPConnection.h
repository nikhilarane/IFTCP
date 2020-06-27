#ifndef _TCPCONNECTION_
#define _TCPCONNECTION_
#include "Socket.h"

namespace IFTCP {
	class TCPConnection {
	public:
		
		TCPConnection(Socket socket, IPEndpoint endPoint);
		void Close();
		std::string ToString();
		Socket mSocket;
	private:
		IPEndpoint mEndPoint;
		std::string mStringRepresentation = "";

	};
}
#endif
