#include <psp2/kernel/processmgr.h>
#include <psp2/sysmodule.h>
#include <sstream>
#include <vector>

#include <cstdio>

#include "debugScreen.h"

#include "./EventLoop/EventLoop.h"

#define printf psvDebugScreenPrintf



int main(int argc, char *argv[]) {
	sceSysmoduleLoadModule(SCE_SYSMODULE_NET);
	
	psvDebugScreenInit();


	auto buf = psvDebugScreenPrintf("Hello, world!\n");

	std::stringstream output;
	std::vector<std::string> hello = { "Hello" };
	hello.push_back(",");
	hello.push_back(" C++ ");
	hello.push_back("world!");
	for (auto &s : hello) {
		// std::cout does't work ATM :(
		output << s;
	}
	output << std::endl;
	printf("%s\n", output.str().c_str());

	using namespace VitaEventLoop;
	EventLoop loop;


	sceKernelDelayThread(30*1000000); // Wait for 30 seconds
	sceKernelExitProcess(0);
    return 0;
}
