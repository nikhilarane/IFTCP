#include "Packet.h"

namespace IFTCP {
	Packet::Packet(PacketType packetType)
	{
		Clear();
		AssignPacketType(packetType);
	}
	PacketType Packet::GetPacketType()
	{
		PacketType *pPacketType = reinterpret_cast<PacketType*>(&mBuffer[0]);
		return static_cast<PacketType>(ntohs(*pPacketType));
	}
	void Packet::AssignPacketType(PacketType packetType)
	{
		PacketType *pPacketType = reinterpret_cast<PacketType*>(&mBuffer[0]);
		*pPacketType = static_cast<PacketType>(htons(packetType));
	}
	void Packet::Clear()
	{
		//mBuffer.clear();
		mBuffer.resize(sizeof(PacketType));
		AssignPacketType(PacketType::PT_Invalid);
		mExtractionOffset = sizeof(PacketType);
	}

	void Packet::Append(const void * data, uint32_t size)
	{
		if ((mBuffer.size() + size) > gMaxPacketSize) {
			throw PacketException::PacketException("Exception : Appending too long a packet\n");
		}
		mBuffer.insert(mBuffer.end(), (char*)data, (char*)data + size);
	}

	Packet& Packet::operator <<(uint32_t data)
	{
		// TODO: insert return statement here
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	Packet& Packet::operator >> (uint32_t & data)
	{
		// TODO: insert return statement here
		if ((mExtractionOffset + sizeof(uint32_t)) > mBuffer.size()) {
			throw PacketException::PacketException("Not enough data in buffer");
		}
		data = *reinterpret_cast<uint32_t*>(&mBuffer[mExtractionOffset]);
		data = ntohl(data);
		mExtractionOffset += sizeof(uint32_t);
		return *this;
	}

	Packet & Packet::operator<<(const char data)
	{
		Append(&data, 1);
		return *this;
	}

	Packet & Packet::operator >> (char & data)
	{
		// TODO: insert return statement here
		

		if ((mExtractionOffset + sizeof(char)) > mBuffer.size()) {
			throw PacketException::PacketException("Not enough data in buffer");
		}
		
		memcpy_s(&data, sizeof(char), &mBuffer[mExtractionOffset], sizeof(char));
		mExtractionOffset += sizeof(char);
		return *this;
	}

	Packet & Packet::operator<<(const std::string & data)
	{
		
		(*this) << (uint32_t)data.size();
		Append(data.data(), data.size());
		return *this;
	}

	Packet & Packet::operator >> (std::string& data)
	{
		data.clear();
		uint32_t stringSize = 0;
		(*this) >> stringSize;
		
		if ((mExtractionOffset + stringSize) > mBuffer.size()) {
			throw PacketException::PacketException("Not enough data in buffer");
		}
		data.resize(stringSize);
		data.assign(&mBuffer[mExtractionOffset], stringSize);
		mExtractionOffset += stringSize;
		return *this;
	}

	Packet & Packet::operator<<(const float data)
	{
		uint32_t idata = 0;
		memcpy_s(&idata, sizeof(uint32_t), &data, sizeof(float));
		(*this) << idata;
		return *this;
	}

	Packet & Packet::operator >> (float & data)
	{
		uint32_t idata = 0;
		(*this) >> idata;
		memcpy_s(&data, sizeof(float), &idata, sizeof(uint32_t));
		return *this;
	}

	Packet & Packet::operator<<(const int data)
	{
		uint32_t idata = 0;
		memcpy_s(&idata, sizeof(uint32_t), &data, sizeof(int));
		(*this) << idata;
		return *this;
	}

	Packet & Packet::operator >> (int & data)
	{
		uint32_t idata = 0;
		(*this) >> idata;
		memcpy_s(&data, sizeof(int), &idata, sizeof(uint32_t));
		return *this;
	}

	Packet & Packet::operator<<(const double data)
	{
		uint32_t idata[2] = { 0 };
		memcpy_s(&idata[0], sizeof(double), &data, sizeof(double));
		(*this) << idata[0];
		(*this) << idata[1];
		return *this;
	}

	Packet & Packet::operator >> (double & data)
	{
		uint32_t idata[2] = { 0 };
		(*this) >> idata[0];
		(*this) >> idata[1];
		memcpy_s(&data, sizeof(double), &idata[0], sizeof(double));
		return *this;
	}

	

}
