//
// FileEvents.h - This file is part of the Grinder library
//
// Copyright (c) 2015 Matthew Brush <mbrush@codebrainz.ca>
// All rights reserved.
//

#ifndef FILEEVENTS_H
#define FILEEVENTS_H

#include "./../net_epoll.h"
// #include <poll.h>

namespace VitaEventLoop
{

enum class NetworkEvents
{
	NONE     = 0,
	INPUT    = POLLIN,
	OUTPUT   = POLLOUT,
	ERROR    = POLLERR,
	HANGUP   = POLLHUP,
};

} // namespace VitaEventLoop

static inline int operator&(VitaEventLoop::NetworkEvents ev1, VitaEventLoop::NetworkEvents ev2)
{
	return static_cast<int>(ev1) & static_cast<int>(ev2);
}

static inline int operator&(int ev1, VitaEventLoop::NetworkEvents ev2)
{
	return ev1 & static_cast<int>(ev2);
}

static inline VitaEventLoop::NetworkEvents operator|(VitaEventLoop::NetworkEvents ev1, VitaEventLoop::NetworkEvents ev2)
{
	return static_cast<VitaEventLoop::NetworkEvents>(static_cast<int>(ev1) | static_cast<int>(ev2));
}

#endif // FILEEVENTS_H