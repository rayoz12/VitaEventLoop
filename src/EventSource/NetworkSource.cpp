
#include "./NetworkSource.h"

#include "./../platform/NetLog/NetLog.h"

namespace VitaEventLoop {

NetworkSource::NetworkSource(NetworkInterface& in_socket, int events): 
	socket(in_socket), revents(0), events(events) {
	sourceType = EventSourceType::Network;
}

bool NetworkSource::prepare(long&) { return false; }

bool NetworkSource::check() {
	if (events == 0 || revents == 0)
		return false;
	if (((revents & NetworkEvents::UV_INPUT) && (events & NetworkEvents::UV_INPUT)) ||
		((revents & NetworkEvents::UV_OUTPUT) && (events & NetworkEvents::UV_OUTPUT)) ||
		((revents & NetworkEvents::UV_ERROR) && (events & NetworkEvents::UV_ERROR)) ||
		((revents & NetworkEvents::UV_HANGUP) && (events &  NetworkEvents::UV_HANGUP)))
	{
		return true;
	}
	return false;
};

bool NetworkSource::dispatch(EventHandler &func) { 
	// If there revents input was true
	if (revents & NetworkEvents::UV_INPUT) {
		NetLog_Log(INFO, "[NETWORK] Input Event\n");
		socket.InputEvent();
	}
	if (revents & NetworkEvents::UV_OUTPUT) {
		if (socket.CanOutput()) {
			NetLog_Log(INFO, "[NETWORK] Output Event\n");
			socket.OutputEvent();
		}
	}
	return func(*this); 
}

}