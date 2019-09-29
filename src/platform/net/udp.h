#ifndef PLATFORM_NET_H
#define PLATFORM_NET_H

#include "./../../EventEmitter/EventEmitter.hpp"

namespace Platform
{

// State to 
enum SocketState {
    INITIAL,
    BINDED,
    CONNECTED
};

struct RemoteAddress {
    std::string address;
    std::string family;
    int port;
    int size;
};

using RemoteAddress_t = struct RemoteAddress;

using CloseCallback=std::function<void()>;
using ConnectCallback=std::function<void()>;
using ErrorCallback=std::function<void(std::string)>;
using ListeningCallback=std::function<void()>;
using MessageCallback=std::function<void(void* buf, size_t len, RemoteAddress_t rinfo)>;

class UDPSocket : EventEmitter
{
public:
	UDPSocket();

    void bind(int port, std::string address, const ListeningCallback& f = {});
    void bind(int port, std::string address, const ListeningCallback& f = {});

private:
	bool isServerSocket = false;
    bool isOpen = false;
    SocketState SocketState;
};

} // namespace Platform

#endif // PLATFORM_NET_H


