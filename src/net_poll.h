#ifndef NET_POLL_H
#define NET_POLL_H

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
int net_poll(struct pollfd *fds, nfds_t nfds, int timeout);

#endif