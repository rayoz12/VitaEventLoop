#ifndef PLATFORM_H
#define PLATFORM_H

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include "malloc.h"
#include <psp2/sysmodule.h>

int err;

//--------------Net Defines-----------------
#define NET_INIT_SIZE 1*1024*1024
#define BUFSIZE 1024

static void* net_memory = NULL;
static char vita_ip[16];
static bool isNet = false;
//--------------Net Defines-----------------


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
	isNet = true;
	return 0;
}

int platform_init() {
	if (err = sceSysmoduleLoadModule(SCE_SYSMODULE_NET) < 0) {
		return err;
	};
	return net_init();
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

int platform_term() {
	if (err = sceSysmoduleUnloadModule(SCE_SYSMODULE_NET) < 0) {
		return err;
	};
	return net_term();
}

#endif