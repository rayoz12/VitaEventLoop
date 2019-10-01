#include <psp2/kernel/processmgr.h>

#include <sstream>
#include <vector>

#include <cstdio>

#include "./EventLoop/EventLoop.h"
#include "./EventSource/TimeoutSource.h"
#include "./EventSource/NetworkSource.h"
#include "./platform/platform.h"
#include "./platform/net/udp.h"

#include "debugScreen.h"
#define printf psvDebugScreenPrintf

void graceful_exit() {
	printf("Exiting");
	platform_term();
	sceKernelDelayThread(30*1000000); // Wait for 30 seconds
	sceKernelExitProcess(0);
}
static int counter = 0;
int main(int argc, char *argv[]) {
	if (platform_init() < 0) {
		printf("Failed to init\n");
		graceful_exit();
	};
	
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

	auto timeoutSource = new TimeoutSource(2000);
	loop.add_event_source(timeoutSource, [&](EventSource&) {
		printf("Timeout Expired\n");
		return false;
	});

	// if (net_CreateUDPServer(4200, &serverSocket) < 0) {
	// 	printf("Failed to create server\n");
	// 	graceful_exit();
	// }

	Platform::UDPSocket udpSocket;
	
	auto networkSource = new NetworkSource(udpSocket, NetworkEvents::INPUT);
	loop.add_event_source(networkSource, [&](EventSource&) {
		printf("activity on network %d\n", ++counter);
		return true;
	});
	printf("Server Started\n");
	loop.run();

	printf("Event Loop Finished\n");

	sceKernelDelayThread(30*1000000); // Wait for 30 seconds
	sceKernelExitProcess(0);

    return 0;
}
