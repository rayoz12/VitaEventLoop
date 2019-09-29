
#ifndef NETWORKSOURCE_H
#define NETWORKSOURCE_H

#include "./EventSource.h"
#include "./NetworkEvents.h"


namespace VitaEventLoop
{
class NetworkSource : public EventSource
{
public:
	int fd, revents;
	NetworkEvents events;

	NetworkSource(int fd, NetworkEvents events)
		: fd(fd), revents(0), events(events) {
        sourceType = EventSourceType::Network;
    }

	bool prepare(long&) override { return false; }

    bool check() override {
        if (events == NetworkEvents::NONE || revents == 0)
			return false;
		if (((revents & NetworkEvents::INPUT) && (events & NetworkEvents::INPUT)) ||
			((revents & NetworkEvents::OUTPUT) && (events & NetworkEvents::OUTPUT)) ||
			((revents & NetworkEvents::ERROR) && (events & NetworkEvents::ERROR)) ||
			((revents & NetworkEvents::HANGUP) && (events &  NetworkEvents::HANGUP)))
		{
			return true;
		}
		return false;
    };

	bool dispatch(EventHandler &func) { return func(*this); }

};

} // namespace Grinder

#endif // NETWORKSOURCE_H