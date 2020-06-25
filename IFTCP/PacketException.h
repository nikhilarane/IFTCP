#ifndef _PACKETEXCEPTION_
#define _PACKETEXCEPTION_
#include <string>

namespace IFTCP {
	class PacketException {
	public:
		PacketException(std::string exception) :mException(exception) {

		}
		const char* what() {
			return mException.c_str();
		}
		std::string ToString() {
			return mException;
		}
	private:
		std::string mException;
	};

}

#endif
