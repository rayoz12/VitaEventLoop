#include "./net_poll.h"
#include <psp2/kernel/threadmgr.h>
#include <psp2/net/net.h>
#include <psp2/types.h>

#define MAX_EVENTS 255
#define MAX_OPEN_FILES 256


/**
 * Poll given net SCE sockets
 * 
 * timeout is in Milliseconds
 * 
 */
int net_poll(struct pollfd *fds, nfds_t nfds, int timeout) {

    if (nfds < 0 || nfds >= MAX_OPEN_FILES) {
		// errno = EINVAL;
		return -1;
	}

	// convert from milliseconds to microseconds
	uint64_t wait = timeout * 1000;

	if (nfds == 0) {
		sceKernelDelayThread(timeout * 1000);
		return 0;
	}

	int i;
	int eid = sceNetEpollCreate("", 0);
	if (eid < 0) {
		// errno = __vita_sce_errno_to_errno(eid);
		return -1;
	}

	for (i = 0; i < nfds; i++) {
		SceNetEpollEvent ev = {0};
		pollfd fd = fds[i];
		ev.data.fd = fd.fd;

		ev.events = fd.events;
		if (ev.events == 0) {
			continue;
		}


		sceNetEpollControl(eid, SCE_NET_EPOLL_CTL_ADD, ev.data.fd, &ev);
	}

	SceNetEpollEvent events[MAX_EVENTS] = {0};
	int nev = sceNetEpollWait(eid, events, MAX_EVENTS, wait); // wait should be in microseconds
	int res = 0;
	if (nev < 0) {
		res = -1;
		goto exit;
	}

	for (i = 0; i < nev; i++) {
		if (events[i].events) {
			fds[i].revents = events[i].events;
			res++;
		}
	}

exit:
	sceNetEpollDestroy(eid);
	return res;
}