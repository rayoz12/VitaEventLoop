#ifndef TIMEOUTSOURCE_H
#define TIMEOUTSOURCE_H

#include "EventSource.h"
#include <cstdint>

namespace VitaEventLoop
{

class TimeoutSource : public EventSource
{
public:
	TimeoutSource(long timeout=-1);
	bool prepare(long &max_timeout) override;
	bool check() override;
	bool dispatch(EventHandler &func) override { return func(*this); }

private:
	long m_timeout;
	std::uint64_t m_next_expiry;
	bool is_ready(long& max_timeout);
};

} // namespace VitaEventLoop

#endif // TIMEOUTSOURCE_H