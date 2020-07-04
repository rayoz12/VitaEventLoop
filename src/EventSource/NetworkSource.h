
#ifndef NETWORKSOURCE_H
#define NETWORKSOURCE_H

#include "./EventSource.h"
#include "./NetworkEvents.h"
#include "./INetworkInterface.h"

namespace VitaEventLoop
{

class NetworkSource : public EventSource
{
public:
	int revents, events;
	NetworkInterface& socket;
	// NetworkEvents events;

	NetworkSource(NetworkInterface& in_socket, int events);

	bool prepare(long&) override;

    bool check() override;

	bool dispatch(EventHandler &func);

};

} // namespace Grinder

#endif // NETWORKSOURCE_H