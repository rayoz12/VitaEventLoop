#ifndef NETLOG_H
#define NETLOG_H

#include <debugnet.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define NetLog_Log(LEVEL, f_, ...) debugNetPrintf(LEVEL, (f_), ##__VA_ARGS__)

int NetLog_init(const char* server_ip, const int server_port);

int NetLog_term();



#ifdef __cplusplus
}
#endif

#endif