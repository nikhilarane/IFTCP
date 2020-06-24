#include "Packet.h"

namespace IFTCP {
	void Packet::Clear()
	{
		mBuffer.clear();
		mExtractionOffset = 0;
	}

	void Packet::Append(const void * data, uint32_t size)
	{
		mBuffer.insert(mBuffer.end(), (char*)data, (char*)data + size);
	}

	Packet& Packet::operator<<(uint32_t data)
	{
		// TODO: insert return statement here
		data = htonl(data);
		Append(&data, sizeof(uint32_t));
		return *this;
	}

	Packet& Packet::operator >> (uint32_t & data)
	{
		// TODO: insert return statement here
		data = *reinterpret_cast<uint32_t*>(&mBuffer[mExtractionOffset]);
		data = ntohl(data);
		mExtractionOffset += sizeof(uint32_t);
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
		data.resize(stringSize);
		data.assign(&mBuffer[mExtractionOffset], stringSize);
		mExtractionOffset += stringSize;
		return *this;
	}

}
