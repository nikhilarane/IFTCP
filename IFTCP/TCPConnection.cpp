#include "TCPConnection.h"

namespace IFTCP {
	TCPConnection::TCPConnection(Socket socket, IPEndpoint endPoint):mSocket(socket),mEndPoint(endPoint)
	{
		mStringRepresentation = "[" + mEndPoint.GetIPString();
		mStringRepresentation += ":" + std::to_string(mEndPoint.GetPort()) + "]";
	}

	void TCPConnection::Close()
	{
		mSocket.Close();
	}

	std::string TCPConnection::ToString()
	{
		return mStringRepresentation;
	}
}

