#ifndef PLATFORM_NET_H
#define PLATFORM_NET_H

#include <string>
#include <functional>
#include <vector>
#include <sys/socket.h>

#include "./../common/common.h";

//Design
/**
 * # Similar to LibUV API except with C++ concepts.
 * 
 * This class provides an abstraction layer over a UDP socket for the platform.
 * The methods below have to be implemented to form a functioning UDP socket.
 * 
 * ## Interaction with the EventLoop.
 * This class will be created by a user and then assigned to a user created network source.
 * The network source should work for both TCP and UDP connections because it relies on a
 * C++ socket interface.
 * When the Network source detects a change in the FD it will call the interface's data
 * recieved function to notify the underlying socket about data available to read.
 * 
 * ## SocketInterface (The NetworkSource will have an instance of this and call methods on it)
 * Methods:
 * StartReceiving (this will also implicily *enable* events on the Source)
 * StopReceiving (this will also implicily *disable* events on the Source)
 * DataReadyToRead
 * DataReadyToWrite(? maybe)
 */

namespace Platform
{

typedef enum uv_membership {
    UV_LEAVE_GROUP = 0,
    UV_JOIN_GROUP
} uv_membership;


class UDPSocket
{
public:
	UDPSocket();

    int open(int socket);

    int setSockOpt();
    
    int bind(int port, const struct sockaddr& addr);

    int connect(const struct sockaddr& addr);

    int getPeerName(struct sockaddr& addr);

    int getSockName(struct sockaddr& addr);

    int setMembership(const std::string& multicast_addr, const std::string& interface_addr, uv_membership membership);

    int setMulticastLoop(bool on);
    int setMulticastTTL(bool on);
    int setMulticastInterface(const std::string& interface_addr);

    int setBroadcast(bool on);
    int setTTL(int ttl);

    // For connected sockets sockaddr must be nullptr, for unconnected sockets it must be defined
    int send(std::vector<buf>, const struct sockaddr& addr);

    int rec


private:
    int socket;
	bool isServerSocket = false;
    bool isOpen = false;
};

} // namespace Platform

#endif // PLATFORM_NET_H


