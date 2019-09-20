#ifndef TIMEOUTSOURCE_H
#define TIMEOUTSOURCE_H

#include "EventSource.h"
#include <cstdint>

namespace VitaEventLoop
{

class TimeoutSource : public EventSource
{
public:
	TimeoutSource(int timeout=-1);
	bool prepare(int &max_timeout) override;
	bool check() override;
	bool dispatch(EventHandler &func) override { return func(*this); }

private:
	int m_timeout;
	std::uint64_t m_next_expiry;
	bool is_ready(int& max_timeout);
};

} // namespace VitaEventLoop

#endif // TIMEOUTSOURCE_H