#ifndef _IFTCP_
#define _IFTCP_
#define WINDOWS_LEAN_AND_MEAN
#include <WinSock2.h>
namespace IFTCP {

	int Foo(int i);
	class Network {
	public:
		static bool Initialise();
		static void Shutdown();
	};
	
}

#endif

//Things learned
//1) Empty project -> All configurations All platforms
//2) Include = $(SolutionDir)
//3) LibDir = $(OutDir)
//4) Solution -> Project build order, Project dependencies
//5) Project properties -> Muilti processor compilation(for faster compilation)
