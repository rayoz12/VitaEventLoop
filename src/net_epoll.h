#ifndef net_epoll_H
#define net_epoll_H

#ifdef __cplusplus
extern "C" {
#endif

#include <psp2/net/net.h>

/* Event types that can be polled for.  These bits may be set in `events'
   to indicate the interesting event types; they will appear in `revents'
   to indicate the status of the file descriptor.  */
#define POLLIN		SCE_NET_EPOLLIN		/* There is data to read.  */
#define POLLOUT		SCE_NET_EPOLLOUT	/* Writing now will not block.  */
//#define POLLPRI		0x002		    /* There is urgent data to read.  */


/* Event types always implicitly polled for.  These bits need not be set in
   `events', but they will appear in `revents' to indicate the status of
   the file descriptor.  */
#define POLLERR		SCE_NET_EPOLLERR		/* Error condition.  */
#define POLLHUP		SCE_NET_EPOLLHUP		/* Hung up.  */

/* Data structure describing a polling request.  */
typedef struct pollfd
{
    int fd;			/* File descriptor to poll.  */
    short int events;		/* Types of events poller cares about.  */
    short int revents;		/* Types of events that actually occurred.  */
} pollfd;

/* Type used for the number of file descriptors.  */
typedef unsigned long int nfds_t;

/**
 * Poll given net SCE sockets
 * 
 * timeout is in Milliseconds
 * 
 */
int net_epoll(struct pollfd *fds, nfds_t nfds, int timeout);

#ifdef __cplusplus
}
#endif

#endif