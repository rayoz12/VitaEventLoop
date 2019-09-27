#include "TimeoutSource.h"
#include "./../Utility.h"
#include <iostream>

#include "debugScreen.h"
#define printf psvDebugScreenPrintf

using namespace std;

namespace VitaEventLoop
{

TimeoutSource::TimeoutSource(long timeout)
	: m_timeout(timeout),
	  m_next_expiry(time_now() + timeout)
{
    sourceType = EventSourceType::Timeout;
}

bool TimeoutSource::is_ready(long &max_timeout)
{
	uint64_t now = time_now();
	// printf("now: %lld, m_next_expiry: %lld\n", now, m_next_expiry);
	if (now >= m_next_expiry)
	{
		m_next_expiry = (now + m_timeout) - (now - m_next_expiry);
		return true;
	}
	else
	{
		max_timeout = m_next_expiry - now;
		return false;
	}
}

bool TimeoutSource::prepare(long &max_timeout)
{
	return is_ready(max_timeout);
}

bool TimeoutSource::check()
{
	long unused = 0;
	return is_ready(unused);
}

} // namespace Event Loop