#ifndef PLATFORM_NET_H
#define PLATFORM_NET_H

#include <string>
#include <functional>
#include <vector>
#include <queue>
#include <sys/socket.h>

#include "./../common/common.h"
#include "./../../EventSource/INetworkInterface.h"
#include "./../../EventLoop/EventLoopMisc.h"

// The Size of the buffer used when sending and receiving data.
#define IO_BUFFER_SIZE 4096

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

typedef struct Remote_Info {
    std::string address;
    std::string family;
    int port;
    int size;
} Remote_Info;

/**
 * This handler MUST copy and data it needs as the buffer is deleted after the call
 */
using ReceiveHandler = std::function<void(int bytesRead, VitaEventLoop::vita_uv_buf& buf, Remote_Info info)>;
using MessageSentHandler = std::function<void(int status)>;

class UDPSocket : public VitaEventLoop::NetworkInterface
{
public:
	UDPSocket();

    // int open(int socket);

    int setSockOpt();
    
    int bind(int port);

    int connect(int port, std::string ip);

    int getPeerName(struct SceNetSockaddr* addr, unsigned int* addrLen);

    int getSockName(struct SceNetSockaddr& addr);

    int setMembership(const std::string& multicast_addr, const std::string& interface_addr, uv_membership membership);

    int setMulticastLoop(bool on);
    int setMulticastTTL(bool on);
    int setMulticastInterface(const std::string& interface_addr);

    int setBroadcast(bool on);
    int setTTL(int ttl);

    // For connected sockets port and ip and 0 and "" respectively
    // Memory allocated in bufs must always be allocated with malloc
    int sendMessage(std::vector<VitaEventLoop::vita_uv_buf *>& bufs, const int port, const std::string ip, MessageSentHandler func);

    void recvStart(ReceiveHandler handler);

    virtual int StartSocket() override;

    virtual int StopSocket() override;

    virtual int InputEvent() override;

    virtual int OutputEvent() override;

    virtual int GetFD() override;

    virtual bool CanOutput() override;



private:
    int sockfd;

    // Flags describing the currect connection. 
    bool isBound = false;
    bool isConnected = false;
    bool isInit = false;
    bool isListening = false;
    bool isServerSocket = false;
    bool isClientSocket = false;


    ReceiveHandler messageHandler;

    SceNetSockaddrIn connectedPeer; //not really because UDP is connectionless
    
    int recvMessageEvent();
    int sendMessageEvent();


    struct SendMsgStruct {
        SendMsgStruct(std::vector<VitaEventLoop::vita_uv_buf *>& bufs_in): bufs(bufs_in) {};
        std::vector<VitaEventLoop::vita_uv_buf *>& bufs;
        std::string ip;
        int port;
        MessageSentHandler cb;
    };

    std::queue<struct SendMsgStruct> sendQueue;

};

} // namespace Platform

#endif // PLATFORM_NET_H


