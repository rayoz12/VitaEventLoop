#ifndef PLATFORM_NET_H
#define PLATFORM_NET_H

#include "./../../EventEmitter/EventEmitter.hpp"

namespace Platform
{

class UDPSocket : EventEmitter
{
public:
	UDPSocket();

    void bind(int port)

private:
	bool isServerSocket = false;
    bool isOpen = false;

};

} // namespace Platform

#endif // PLATFORM_NET_H


