
#include "./udp.h"

#include <sys/fcntl.h>

using namespace Platform;
using namespace std;
using namespace VitaEventLoop;

UDPSocket::UDPSocket() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return;
    }
    isInit = true;
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
}

// int open(int socket) {
    
// }

int UDPSocket::setSockOpt() {
    return 0;
}

int UDPSocket::bind(int port, const struct sockaddr_in& addr) {
    int err = ::bind(sockfd, (sockaddr*) &addr, sizeof(addr));
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
    struct sockaddr_in remote;
    socklen_t addrLen;

    int nbytes = ::recvfrom(sockfd, buf, IO_BUFFER_SIZE, 0, (sockaddr*) &remote, &addrLen);
    if (nbytes < 0) {
        return nbytes;
    }
    uv_buf->len = nbytes;
    messageHandler(nbytes, *uv_buf, remote);
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