
#include "./udp.h"

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include "./../NetLog/NetLog.h"

using namespace Platform;
using namespace std;
using namespace VitaEventLoop;

int __vita_sce_errno_to_errno(int sce_errno);

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

    isServerSocket = true;

    return err;
}

int UDPSocket::connect(int port, std::string ip) {
    connectedPeer.sin_family = SCE_NET_AF_INET;
	connectedPeer.sin_port = sceNetHtons(port);
    int err = ::sceNetInetPton(SCE_NET_AF_INET, ip.c_str(), &connectedPeer.sin_addr);
    if (err < 0) {
        return err; 
    }
    err = ::sceNetConnect(sockfd, (SceNetSockaddr* ) &connectedPeer, sizeof(connectedPeer));
    if (err < 0) {
        NetLog_Log(ERROR, "Failed to connect to socket");
        return err;
    }
    isClientSocket = true;
    isConnected = true;
    return err;
}

int UDPSocket::getPeerName(struct SceNetSockaddr* addr, unsigned int* addrLen) {
    int ret = ::sceNetGetpeername(sockfd, addr, addrLen);
    if (ret < 0) {
        NetLog_Log(ERROR, "Failed to Get Peer Name");
    }
    return ret;
}

int UDPSocket::getSockName(struct SceNetSockaddr& addr) {
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
// for connected sockets port and ip are ignored
int UDPSocket::sendMessage(std::vector<VitaEventLoop::vita_uv_buf *>& bufs, const int port, const std::string ip, MessageSentHandler func) {
    if (isConnected && port != 0 && ip != "")
        return EINVAL;
    
    SendMsgStruct msg(bufs);
    // even tho a connected socket will never need to specify
    msg.ip = ip;
    msg.port = port;
    if (func) {
        msg.cb = func;
    }
    sendQueue.push(msg);
    NetLog_Log(INFO, "Placed into sendqueue\n");
	return 0;
    return OutputEvent(); //this just tries to send data in the send queue;
}

int UDPSocket::recvMessageEvent() {
    //allocate the buffer
    void* buf = malloc(IO_BUFFER_SIZE);
	memset(buf, 0, IO_BUFFER_SIZE);
    vita_uv_buf* uv_buf = new vita_uv_buf;
    uv_buf->base = (char*) buf;

    //allocate the remote address
    struct SceNetSockaddrIn remote;
    unsigned int addrLen = sizeof(remote);

    int nbytes = ::sceNetRecvfrom(sockfd, buf, IO_BUFFER_SIZE, 0, (SceNetSockaddr*) &remote, &addrLen);
    if (nbytes < 0) {
        NetLog_Log(ERROR, "Failed to Read bytes ERR:%d\n", __vita_sce_errno_to_errno(nbytes));
        return nbytes;
    }
    uv_buf->len = nbytes;

    // populate remote info
    Remote_Info info;
    char s[INET_ADDRSTRLEN];
    const char* err = sceNetInetNtop(SCE_NET_AF_INET, &(remote.sin_addr.s_addr), &s[0], INET_ADDRSTRLEN);
    if (err != NULL) {
        //NetLog_Log(INFO, "IP: %s\n", s);
        info.address = std::string(err);
    }
    else {
        info.address = "unknown";
    }
    info.family = "IPv4";
    info.port = remote.sin_port;
    info.size = nbytes;

    if (messageHandler) {
        messageHandler(nbytes, *uv_buf, info);
    }
    else {
        NetLog_Log(DEBUG, "Message Handler not defined\n");
    }
    free(buf);
    buf = nullptr;
    delete uv_buf;
    return 0;
}

// For connected sockets sockaddr must be nullptr, for unconnected sockets it must be defined
// for connected sockets port and ip are ignored
int UDPSocket::sendMessageEvent() {
    // callout by the event loop when data is ready to write.
    int err;
    while(!sendQueue.empty()) {
        NetLog_Log(INFO, "Sending Data, Send Queue Size: %d\n", sendQueue.size());
        auto firstMessage = sendQueue.front();
        NetLog_Log(INFO, "FirstMessage: Base:%p, len: %d\n", &firstMessage.bufs[0]->base, firstMessage.bufs[0]->len);
        SceNetMsghdr msg;
        if (isConnected) {
            msg.msg_name = nullptr;
            msg.msg_namelen = 0;
        }
        else {
            SceNetSockaddrIn addr;
            err = vita_uv_ip4_addr(firstMessage.ip.c_str(), firstMessage.port, &addr);
            if (err < 0) {
                NetLog_Log(ERROR, "Failed to get IPv4 Addr: %d\n", err);
                return err;
            }
            // NetLog_Log(INFO, "ADDR: %d", addr.sin_addr.s_addr);
            msg.msg_name = &addr;
            msg.msg_namelen = sizeof(addr);
        }
        SceNetIovec io[255];
        for (size_t i = 0; i < firstMessage.bufs.size(); i++)
        {
		    NetLog_Log(INFO, "BUF Size: %d\n", firstMessage.bufs.size());
            io[i].iov_base = firstMessage.bufs[i]->base;
            io[i].iov_len = firstMessage.bufs[i]->len;
            NetLog_Log(INFO, "len: %d, base: %p\n", firstMessage.bufs[i]->len, &firstMessage.bufs[i]->base);
        }
        msg.msg_iovlen = firstMessage.bufs.size();

        ssize_t size;
        int ret;
        size = sceNetSendmsg(sockfd, &msg, 0);
        if (size < 0) {
            // send buffer is full
            if (size == SCE_NET_ERROR_EWOULDBLOCK || 
                size == SCE_NET_ERROR_EAGAIN) {
                ret = 0;
                NetLog_Log(ERROR, "Socket Send would block, exiting\n");
                break;
            }
            else {
                // Error
                int err = __vita_sce_errno_to_errno(size);
                NetLog_Log(ERROR, "Failed to Send: SCE: %d, POSIX: %d\n", size, err);
                ret = -err;
            }
        }
        firstMessage.cb(ret);
        for (size_t i = 0; i < firstMessage.bufs.size(); i++)
        {
            free(firstMessage.bufs[i]->base);
            firstMessage.bufs[i]->base = nullptr;
        }
        sendQueue.pop();
        return ret;
    }
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
    return recvMessageEvent();
}

int UDPSocket::OutputEvent() {
    return sendMessageEvent();
}

bool UDPSocket::CanOutput() {
    return !sendQueue.empty();
}

int UDPSocket::GetFD() {
    return sockfd;
}


int __vita_sce_errno_to_errno(int sce_errno)
{
	switch (sce_errno)
	{
		case SCE_NET_ERROR_EPERM:
			return EPERM;
		case SCE_NET_ERROR_ENOENT:
			return ENOENT;
		case SCE_NET_ERROR_ESRCH:
			return ESRCH;
		case SCE_NET_ERROR_EINTR:
			return EINTR;
		case SCE_NET_ERROR_EIO:
			return EIO;
		case SCE_NET_ERROR_ENXIO:
			return ENXIO;
		case SCE_NET_ERROR_E2BIG:
			return E2BIG;
		case SCE_NET_ERROR_ENOEXEC:
			return ENOEXEC;
		case SCE_NET_ERROR_EBADF:
			return EBADF;
		case SCE_NET_ERROR_ECHILD:
			return ECHILD;
		case SCE_NET_ERROR_EDEADLK:
			return EDEADLK;
		case SCE_NET_ERROR_ENOMEM:
			return ENOMEM;
		case SCE_NET_ERROR_EACCES:
			return EACCES;
		case SCE_NET_ERROR_EFAULT:
			return EFAULT;
		case SCE_NET_ERROR_ENOTBLK:
			return ENOTBLK;
		case SCE_NET_ERROR_EBUSY:
			return EBUSY;
		case SCE_NET_ERROR_EEXIST:
			return EEXIST;
		case SCE_NET_ERROR_EXDEV:
			return EXDEV;
		case SCE_NET_ERROR_ENODEV:
			return ENODEV;
		case SCE_NET_ERROR_ENOTDIR:
			return ENOTDIR;
		case SCE_NET_ERROR_EISDIR:
			return EISDIR;
		case SCE_NET_ERROR_EINVAL:
			return EINVAL;
		case SCE_NET_ERROR_ENFILE:
			return ENFILE;
		case SCE_NET_ERROR_EMFILE:
			return EMFILE;
		case SCE_NET_ERROR_ENOTTY:
			return ENOTTY;
		case SCE_NET_ERROR_ETXTBSY:
			return ETXTBSY;
		case SCE_NET_ERROR_EFBIG:
			return EFBIG;
		case SCE_NET_ERROR_ENOSPC:
			return ENOSPC;
		case SCE_NET_ERROR_ESPIPE:
			return ESPIPE;
		case SCE_NET_ERROR_EROFS:
			return EROFS;
		case SCE_NET_ERROR_EMLINK:
			return EMLINK;
		case SCE_NET_ERROR_EPIPE:
			return EPIPE;
		case SCE_NET_ERROR_EDOM:
			return EDOM;
		case SCE_NET_ERROR_ERANGE:
			return ERANGE;
		case SCE_NET_ERROR_EAGAIN:
			return EAGAIN;
// these values are the same as EAGAIN
//		case SCE_NET_ERROR_EWOULDBLOCK:
//			return EWOULDBLOCK;
		case SCE_NET_ERROR_EINPROGRESS:
			return EINPROGRESS;
		case SCE_NET_ERROR_EALREADY:
			return EALREADY;
		case SCE_NET_ERROR_ENOTSOCK:
			return ENOTSOCK;
		case SCE_NET_ERROR_EDESTADDRREQ:
			return EDESTADDRREQ;
		case SCE_NET_ERROR_EMSGSIZE:
			return EMSGSIZE;
		case SCE_NET_ERROR_EPROTOTYPE:
			return EPROTOTYPE;
		case SCE_NET_ERROR_ENOPROTOOPT:
			return ENOPROTOOPT;
		case SCE_NET_ERROR_EPROTONOSUPPORT:
			return EPROTONOSUPPORT;
		case SCE_NET_ERROR_ESOCKTNOSUPPORT:
			return ESOCKTNOSUPPORT;
		case SCE_NET_ERROR_EOPNOTSUPP:
			return EOPNOTSUPP;
		case SCE_NET_ERROR_EPFNOSUPPORT:
			return EPFNOSUPPORT;
		case SCE_NET_ERROR_EAFNOSUPPORT:
			return EAFNOSUPPORT;
		case SCE_NET_ERROR_EADDRINUSE:
			return EADDRINUSE;
		case SCE_NET_ERROR_EADDRNOTAVAIL:
			return EADDRNOTAVAIL;
		case SCE_NET_ERROR_ENETDOWN:
			return ENETDOWN;
		case SCE_NET_ERROR_ENETUNREACH:
			return ENETUNREACH;
		case SCE_NET_ERROR_ENETRESET:
			return ENETRESET;
		case SCE_NET_ERROR_ECONNABORTED:
			return ECONNABORTED;
		case SCE_NET_ERROR_ECONNRESET:
			return ECONNRESET;
		case SCE_NET_ERROR_ENOBUFS:
			return ENOBUFS;
		case SCE_NET_ERROR_EISCONN:
			return EISCONN;
		case SCE_NET_ERROR_ENOTCONN:
			return ENOTCONN;
		case SCE_NET_ERROR_ESHUTDOWN:
			return ESHUTDOWN;
		case SCE_NET_ERROR_ETOOMANYREFS:
			return ETOOMANYREFS;
		case SCE_NET_ERROR_ETIMEDOUT:
			return ETIMEDOUT;
		case SCE_NET_ERROR_ECONNREFUSED:
			return ECONNREFUSED;
		case SCE_NET_ERROR_ELOOP:
			return ELOOP;
		case SCE_NET_ERROR_ENAMETOOLONG:
			return ENAMETOOLONG;
		case SCE_NET_ERROR_EHOSTDOWN:
			return EHOSTDOWN;
		case SCE_NET_ERROR_EHOSTUNREACH:
			return EHOSTUNREACH;
		case SCE_NET_ERROR_ENOTEMPTY:
			return ENOTEMPTY;
		case SCE_NET_ERROR_EUSERS:
			return EUSERS;
		case SCE_NET_ERROR_EDQUOT:
			return EDQUOT;
		case SCE_NET_ERROR_ESTALE:
			return ESTALE;
		case SCE_NET_ERROR_EREMOTE:
			return EREMOTE;
		case SCE_NET_ERROR_ENOLCK:
			return ENOLCK;
		case SCE_NET_ERROR_ENOSYS:
			return ENOSYS;
		case SCE_NET_ERROR_EIDRM:
			return EIDRM;
		case SCE_NET_ERROR_EOVERFLOW:
			return EOVERFLOW;
		case SCE_NET_ERROR_EILSEQ:
			return EILSEQ;
		case SCE_NET_ERROR_ENOTSUP:
			return ENOTSUP;
		case SCE_NET_ERROR_ECANCELED:
			return ECANCELED;
		case SCE_NET_ERROR_EBADMSG:
			return EBADMSG;
		case SCE_NET_ERROR_ENODATA:
			return ENODATA;
		case SCE_NET_ERROR_ENOSR:
			return ENOSR;
		case SCE_NET_ERROR_ENOSTR:
			return ENOSTR;
		case SCE_NET_ERROR_ETIME:
			return ETIME;
		default:
			return sce_errno;
	}
}