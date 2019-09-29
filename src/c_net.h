
#ifndef NET_H
#define NET_H

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include "malloc.h"

#include "debugScreen.h"
#define printf psvDebugScreenPrintf

#define NET_INIT_SIZE 1*1024*1024
#define BUFSIZE 1024

static void* net_memory = NULL;
static char vita_ip[16];
static bool isNet = false;

typedef struct
{
	int sock;
	bool serverSocket;
} Socket;

int net_init() {
	int ret = sceNetShowNetstat();
	SceNetInitParam initparam;
	if (ret == SCE_NET_ERROR_ENOTINIT) {
		net_memory = malloc(NET_INIT_SIZE);
		initparam.memory = net_memory;
		initparam.size = NET_INIT_SIZE;
		initparam.flags = 0;
		ret = sceNetInit(&initparam);
		if (ret < 0) return ret;
	}
    sceNetCtlInit();
	isNet = 1;
	return 0;
}

int net_term() {
	sceNetTerm();
    sceNetCtlTerm();
	if (net_memory != NULL) 
        free(net_memory);
	net_memory = NULL;
	isNet = 0;
	return 0;
}

int net_CreateUDPServer(int port, Socket** socketPtr) {
	int type = SCE_NET_IPPROTO_UDP;
	
	Socket* my_socket = (Socket*) malloc(sizeof(Socket));
	my_socket->serverSocket = true;

	my_socket->sock = sceNetSocket("Server Socket", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP);
	if (my_socket->sock <= 0) {
        printf("Failed to create socket\n");
        return my_socket->sock;
    }

	int _true = 1;
	SceNetSockaddrIn addrTo;
	addrTo.sin_family = SCE_NET_AF_INET;
	addrTo.sin_port = sceNetHtons(port);
    SceNetCtlInfo info;
    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info);
    sceNetInetPton(SCE_NET_AF_INET, info.ip_address, &addrTo.sin_addr);

    // sceNetInetPton(SCE_NET_AF_INET, "192.168.1.37", &addrTo.sin_port);
	
	int err = sceNetBind(my_socket->sock, (SceNetSockaddr*)&addrTo, sizeof(addrTo));
	if (err != 0) {
        printf("Failed to bind to socket\n");
        return err;
    }

	sceNetSetsockopt(my_socket->sock, SCE_NET_SOL_SOCKET, SCE_NET_SO_NBIO, &_true, sizeof(_true));
	
    *socketPtr = my_socket;

	return 0;
}

// void net_recv(Socket* socket) {
// 	/*
// 	* recvfrom: receive a UDP datagram from a client
// 	*/
// 	buf = malloc(BUFSIZE);
// 	n = recvfrom(sockfd, buf, BUFSIZE, 0,
// 				(struct sockaddr *)&clientaddr, &clientlen);
// 	if (n < 0)
// 		error("ERROR in recvfrom");
// }

#endif