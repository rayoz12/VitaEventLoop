#include "TimeoutSource.h"
#include "./../Utility.h"
#include <iostream>

using namespace std;

namespace VitaEventLoop
{

TimeoutSource::TimeoutSource(int timeout)
	: m_timeout(timeout),
	  m_next_expiry(time_now() + timeout)
{
    sourceType = EventSourceType::Timeout;
}

bool TimeoutSource::is_ready(int &max_timeout)
{
	uint64_t now = time_now();
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

bool TimeoutSource::prepare(int &max_timeout)
{
	return is_ready(max_timeout);
}

bool TimeoutSource::check()
{
	int unused = 0;
	return is_ready(unused);
}

} // namespace Event Loop