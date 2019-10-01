#ifndef COMMON_H
#define COMMON_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

namespace Platform
{
    typedef struct vita_uv_buf {
        char* base;
        int len;
    } vita_uv_buf;

} // namespace Platform

// int vita_uv_ip4_addr(const char* ip, int port, struct sockaddr_in* addr) {
//     memset(addr, 0, sizeof(*addr));
//     addr->sin_family = AF_INET;
//     addr->sin_port = htons(port);
//     return inet_pton(AF_INET, ip, &(addr->sin_addr.s_addr));
// }

#endif // COMMON_H