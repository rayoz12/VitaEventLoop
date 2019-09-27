#ifndef EVENTSOURCE_H
#define EVENTSOURCE_H

#include <functional>

namespace VitaEventLoop
{

class EventLoop;
class EventSource;

typedef std::function<bool(EventSource&)> EventHandler;

enum EventSourceType {
	Timeout,
	Interval,
	Network,
	FileIO
};

class EventSource
{
public:
	virtual ~EventSource() {}

	/**
	 * Called before the event loop polls any file descriptors. If your event source knows
	 * it's ready now, implementations can return true and skip polling. The max_timeout 
	 * argument is an output parameter that can limit how long the poll() timeout is. 
	 * The value is in microseconds and may be lowered by other event sources.
	 * 
	 * @param max_timeout an output parameter that can limit how long the select() timeout is in microseconds
	 */
	virtual bool prepare(long& max_timeout) { max_timeout = -1; return false; }

	/**
	 * This function is called after the file descriptors have been polled. 
	 * The check() function won't be called if the prepare() function returned 
	 * true for a particular event loop iteration. 
	 * Examples of using this function are to check if specific FileEvents have 
	 * occured or whether a timer has elapsed. Return true to have the event handler 
	 * dispatched or false to not dispatch.
	 */
	virtual bool check() = 0;

	/**
	 * This function is called when either the prepare() or check() functions have returned 
	 * true indicating that an event has occurred. The handler argument provides a reference 
	 * to the function to be called. Implementations of this function might choose to read a 
	 * file descriptor or setup some event source-specific state which might be useful to event handlers. 
	 * When this function returns false, the event source dispatching will be removed from 
	 * the event loop and deleted. If it returns true, events will continue to be checked and dispatched.
	 */ 
	virtual bool dispatch(EventHandler& func) = 0;

	EventSourceType sourceType;

private:
	friend class EventLoop;
protected:
	struct {
		bool ready;
		EventHandler handler;
	} loop_data;
};

} // namespace VitaEventLoop

#endif // EVENTSOURCE_H