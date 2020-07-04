#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace Platform
{
    inline int vita_uv_ip4_addr(const char* ip, int port, SceNetSockaddrIn* addr) {
    memset(addr, 0, sizeof(*addr));
    addr->sin_family = SCE_NET_AF_INET;
    addr->sin_port = htons(port);
    return sceNetInetPton(SCE_NET_AF_INET, ip, &(addr->sin_addr));
}


} // namespace Platform


#endif // COMMON_H