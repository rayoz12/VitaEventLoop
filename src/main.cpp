#include <psp2/kernel/processmgr.h>

#include <sstream>
#include <vector>
#include <cstdio>

// #include <sys/socket.h>
// extern "C" {
// 	#include <netinet/in.h>
// 	#include <arpa/inet.h>
// }

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
	udpSocket.recvStart([&](int bytes, vita_uv_buf& buf, Platform::Remote_Info info) {
		printf("\nReceived %s\n", buf.base);
		printf("Remote: %s:%d\n", info.address.c_str(), info.port);
		NetLog_Log(INFO, "Received %s\n", buf.base);
		NetLog_Log(INFO, "Remote: %s:%d\n", info.address.c_str(), info.port);

		vita_uv_buf* copy = new vita_uv_buf;
		copy->base = malloc(buf.len);
		memcpy(copy->base, buf.base, buf.len);
		copy->len = buf.len;
		std::vector<vita_uv_buf *> bufVect;
		bufVect.push_back(copy);
		NetLog_Log(INFO, "Copy: %p\n", &copy->base);
		int err = udpSocket.sendMessage(bufVect, info.port, info.address, [&](int status){
			if (status < 0) {
				NetLog_Log(ERROR, "Error %d\n", status);
				// printf("Error");
			}
			else {
				NetLog_Log(INFO, "Sent %d\n", status);
				// printf("Sent");
			}
			free(copy->base);
			delete copy;
		});
		if (err < 0) {
			NetLog_Log(ERROR, "Failed to Send Message %d\n", err);
		}
	});

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = 4200;
	if (udpSocket.bind(4200) < 0) {
		printf("Failed to bind\n");
		graceful_exit();
	}
	
	auto networkSource = new NetworkSource(udpSocket, static_cast<int>(NetworkEvents::UV_INPUT | NetworkEvents::UV_OUTPUT));
	loop.add_event_source(networkSource, [&](EventSource& source) {
		// printf("activity on network %d\n", ++counter);

		// auto netSource = dynamic_cast<NetworkSource&>(source);
		// netSource.socket.InputEvent();

		return true;
	});
	printf("Server Started\n");
	NetLog_Log(INFO, "Server Started\n");
	loop.run();
	NetLog_Log(INFO, "Event Loop Finished\n");
	printf("Event Loop Finished\n");

	sceKernelDelayThread(30*1000000); // Wait for 30 seconds
	sceKernelExitProcess(0);

    return 0;
}
