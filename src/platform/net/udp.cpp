
#include "./udp.h"

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>

using namespace Platform;
using namespace std;
using namespace VitaEventLoop;

UDPSocket::UDPSocket() {
    // sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    // if (sockfd < 0) {
    //     return;
    // }
    // isInit = true;
    // fcntl(sockfd, F_SETFL, O_NONBLOCK);

    int type = SCE_NET_IPPROTO_UDP;

	sockfd = sceNetSocket("Server Socket", SCE_NET_AF_INET, SCE_NET_SOCK_DGRAM, SCE_NET_IPPROTO_UDP);
	if (sockfd <= 0) {
        //printf("Failed to create socket\n");
        return;
    }
    isInit = true;
}

// int open(int socket) {
    
// }

int UDPSocket::setSockOpt() {
    return 0;
}

int UDPSocket::bind(int port) {
    int _true = 1;
	SceNetSockaddrIn addrTo;
	addrTo.sin_family = SCE_NET_AF_INET;
	addrTo.sin_port = sceNetHtons(port);
    SceNetCtlInfo info;
    sceNetCtlInetGetInfo(SCE_NETCTL_INFO_GET_IP_ADDRESS, &info);
    sceNetInetPton(SCE_NET_AF_INET, info.ip_address, &addrTo.sin_addr);
    // sceNetInetPton(SCE_NET_AF_INET, "192.168.1.37", &addrTo.sin_port);


    int err = ::sceNetBind(sockfd, (SceNetSockaddr*)&addrTo, sizeof(addrTo));
    return err;
}

int UDPSocket::connect(const struct sockaddr& addr) {
    return 0;
}

int UDPSocket::getPeerName(struct sockaddr& addr) {
    return 0;
}

int UDPSocket::getSockName(struct sockaddr& addr) {
    return 0;
}

int UDPSocket::setMembership(const std::string& multicast_addr, const std::string& interface_addr, uv_membership membership) {
    return 0;
}

int UDPSocket::setMulticastLoop(bool on) {
    return 0;
}
int UDPSocket::setMulticastTTL(bool on) {
    return 0;
}
int UDPSocket::setMulticastInterface(const std::string& interface_addr) {
    return 0;
}

int UDPSocket::setBroadcast(bool on) {
    return 0;
}
int UDPSocket::setTTL(int ttl) {
    return 0;
}

void UDPSocket::recvStart(ReceiveHandler handler) {
    messageHandler = handler;
}

// For connected sockets sockaddr must be nullptr, for unconnected sockets it must be defined
int UDPSocket::sendMessage(std::vector<VitaEventLoop::vita_uv_buf>, const struct sockaddr& addr) {
    return 0;
}

int UDPSocket::recvMessage() {
    //allocate the buffer
    void* buf = malloc(IO_BUFFER_SIZE);
    vita_uv_buf* uv_buf = new vita_uv_buf;
    uv_buf->base = (char*) buf;

    //allocate the remote address
    struct SceNetSockaddrIn remote;
    unsigned int addrLen;

    int nbytes = ::sceNetRecvfrom(sockfd, buf, IO_BUFFER_SIZE, 0, (SceNetSockaddr*) &remote, &addrLen);
    if (nbytes < 0) {
        return nbytes;
    }
    uv_buf->len = nbytes;

    // populate remote info
    Remote_Info info;
    char s[256];
    const char* err = sceNetInetNtop(SCE_NET_AF_INET, (void *) &remote.sin_addr.s_addr, &s[0], 256);
    if (err != NULL) {
        info.address = std::string(err);
    }
    else {
        info.address = "unknown";
    }
    info.family = "IPv4";
    info.port = remote.sin_port;
    info.size = nbytes;


    messageHandler(nbytes, *uv_buf, info);
    free(buf);
    delete uv_buf;
    return 0;
}

int UDPSocket::StartSocket() {
    //If the socket is valid an already listening continue as normal.
    //if not initalised, read the current state from flags and then recreate the socket.
    
    return 0;
}

int UDPSocket::StopSocket() {
    //close the socket leaving the flags in tact
    // maybe shutdown()?
    return 0;
}

int UDPSocket::InputEvent() {
    // data is recieved, called by he event loop 
    return recvMessage();
}

int UDPSocket::OutpuEvent() {
    // callout by the event loop when data is ready to write.
    return 0;
}

int UDPSocket::GetFD() {
    return sockfd;
}