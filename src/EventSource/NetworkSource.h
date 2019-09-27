
#ifndef NETWORKSOURCE_H
#define NETWORKSOURCE_H

#include "./EventSource.h"


namespace VitaEventLoop
{
class NetworkSource : public EventSource
{
public:
	int fd;
	
    bool eventOccured;

	NetworkSource(int fd)
		: fd(fd) {
        eventOccured = false;
        sourceType = EventSourceType::Network;
        loop_data.ready = false;
    }

	bool prepare(long&) override { return false; }

    bool check() override {
        return eventOccured;
    };

	bool dispatch(EventHandler &func) { return func(*this); }

};

} // namespace Grinder

#endif // NETWORKSOURCE_H