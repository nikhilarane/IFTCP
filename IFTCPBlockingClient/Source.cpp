#include "Client.h"
#include <iostream>
int mainBlocking() {
	//int val = IFTCP::Foo(10);
	//std::cout << val << std::endl;
	if (IFTCP::Network::Initialise()) {
		std::cout << "initialised\n";
		//IFTCP::Socket sock;
		IFTCP::Socket sock(IFTCP::IPVersion::IPv6);
		if (sock.Create() == IFTCP::PResult::P_Success) {
			std::cout << "Socket Succesfully Created\n";
			//if (sock.Connect(IFTCP::IPEndpoint("127.0.0.1", 9999)) == IFTCP::PResult::P_Success) {
			if (sock.Connect(IFTCP::IPEndpoint("::1", 9999)) == IFTCP::PResult::P_Success) {
				//std::cout << "Socket Succesfully connected\n";
				//char buffer[256];
				//strcpy_s(buffer, "Will finish this today\n");
				//int bytesSent = 0;
				//int result = IFTCP::PResult::P_Success;
				//std::string buffer = "I will finish this today\n";
				//std::string buffer = "";
				//buffer.resize(IFTCP::gMaxPacketSize + 1);
				//memset(&buffer[0], 'A', IFTCP::gMaxPacketSize + 1);
				
				//IFTCP::Packet p;
				//uint32_t a(4), b(5), c(10);
				//p << a << b << c << str;
				//std::string str1("Test sending string\n");
				//std::string str2("Precision Landing\n");
				//int a = -113;
				//float b = -14.768f;
				//p << str1 << str2 << a << b;
				IFTCP::Packet stringPacket(IFTCP::PacketType::PT_ChatMessage);
				stringPacket << "Hello TCP World\n";

				IFTCP::Packet integerArrayPacket(IFTCP::PacketType::PT_IntegerArray);
				const uint32_t array_size = 6;
				int integerArray[array_size] = {-1,-5,0,18,100,52};
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
				while (true) {
					//IFTCP::PResult result = sock.Send(p);
					IFTCP::PResult result;
					int gamble = rand() % 5;
					if ( gamble == 0) {
						result = sock.Send(integerArrayPacket);
					}
					else if (gamble == 1) {
						
						result = sock.Send(stringPacket);
					}
					else if (gamble == 2) {

						result = sock.Send(doubleArrayPacket);
					}
					else if (gamble == 3) {

						result = sock.Send(charArrayPacket);
					}
					else {
						result = sock.Send(floatArrayPacket);
					}
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					std::cout << "Attempting to Send ..." << std::endl;
					Sleep(2000);
					/*uint32_t bufferSize = buffer.size();
					bufferSize = htonl(bufferSize);
					int result = sock.SendAll(&bufferSize, sizeof(uint32_t));
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					result = sock.SendAll(buffer.data(), buffer.size());
					if (result != IFTCP::PResult::P_Success) {
						break;
					}
					std::cout << "Attempting to Send ..." << std::endl;
					Sleep(1000);*/
				}
			}
			else {
				std::cout << "Socket connection failed\n";
			}
			sock.Close();
		}
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}

int main() {
	Client client;
	if (client.Connect(IPEndpoint("::1", 9999))) {
		while (client.IsConnected()) {
			client.Frame();
		}
	}
	IFTCP::Network::Shutdown();
	system("pause");
	return 0;
}