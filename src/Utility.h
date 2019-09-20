#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>
#include <cstdint>

namespace VitaEventLoop
{

static inline std::uint64_t time_now()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count();
}

} // namespace VitaEventLoop

#endif // UTILITY_H