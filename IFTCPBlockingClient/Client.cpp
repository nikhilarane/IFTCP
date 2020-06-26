#include "Client.h"
#include <iostream>

bool Client::Connect(IPEndpoint endPoint)
{
	mIsConnected = false;
	if (Network::Initialise()) {
		std::cout << "initialised\n";
		//IFTCP::Socket sock;
		mSocket = IFTCP::Socket(endPoint.GetIPVersion());
		if (mSocket.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			if (mSocket.SetBlocking(true) != PResult::P_Success) {
				return PResult::P_GenericError;
			}
			//if (sock.Connect(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {
			if (mSocket.Connect(endPoint) == IFTCP::PResult::P_Success) {
				mIsConnected = true;
				return true;
			}
			else {
				std::cout << "Socket connection failed\n";
			}
			mSocket.Close();
		}
		else {
			std::cout << "Failed to create socket\n";
		}
	}
	return false;
}

bool Client::IsConnected()
{
	return mIsConnected;
}

bool Client::Frame()
{
	
	IFTCP::Packet stringPacket(IFTCP::PacketType::PT_ChatMessage);
	stringPacket << "Hello TCP World\n";

	IFTCP::Packet integerArrayPacket(IFTCP::PacketType::PT_IntegerArray);
	const uint32_t array_size = 6;
	int integerArray[array_size] = { -1,-5,0,18,100,52 };
	integerArrayPacket << array_size;
	for (auto var : integerArray)
	{
		integerArrayPacket << var;
	}
	IFTCP::Packet floatArrayPacket(IFTCP::PacketType::PT_FloatArray);
	const uint32_t floatarray_size = 5;
	float floatintegerArray[floatarray_size] = { -1.8f,-5.3f,0.0f,18.99f,100.897f };
	floatArrayPacket << floatarray_size;
	for (auto var : floatintegerArray)
	{
		floatArrayPacket << var;
	}

	IFTCP::Packet doubleArrayPacket(IFTCP::PacketType::PT_DoubleArray);
	const uint32_t doublearray_size = 7;
	double doubleintegerArray[doublearray_size] = { 10.4,-1.8,-5.3,0.0,18.99, 100.897, 6789564.87654 };
	doubleArrayPacket << doublearray_size;
	for (auto var : doubleintegerArray)
	{
		doubleArrayPacket << var;
	}

	IFTCP::Packet charArrayPacket(IFTCP::PacketType::PT_CharArray);
	const uint32_t chararray_size = 2;
	char charArray[chararray_size] = { 'N','R' };
	charArrayPacket << chararray_size;
	for (auto var : charArray)
	{
		charArrayPacket << var;
	}
	
	//IFTCP::PResult result = sock.Send(p);
	IFTCP::PResult result;
	int gamble = rand() % 5;
	if (gamble == 0) {
		result = mSocket.Send(integerArrayPacket);
	}
	else if (gamble == 1) {

		result = mSocket.Send(stringPacket);
	}
	else if (gamble == 2) {

		result = mSocket.Send(doubleArrayPacket);
	}
	else if (gamble == 3) {

		result = mSocket.Send(charArrayPacket);
	}
	else {
		result = mSocket.Send(floatArrayPacket);
	}
	if (result != IFTCP::PResult::P_Success) {
		mIsConnected = false;
		return false;
	}
	std::cout << "Attempting to Send ..." << std::endl;
	Sleep(2000);
	return true;
	
}
