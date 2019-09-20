
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
    }

	bool prepare(long&) override { return false; }

    /**
     * Deleted function please use the <#check(fd_set)> method
     */
    bool check() override {
        return eventOccured;
    };

	bool dispatch(EventHandler &func) { return func(*this); }

};

} // namespace Grinder

#endif // NETWORKSOURCE_H