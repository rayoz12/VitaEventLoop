#include "./NetLog.h"

int NetLog_init(const char* server_ip, const int server_port) {
    int err = debugNetInit(server_ip, server_port, DEBUG);
    if (err < 0)
        return err;
    
    NetLog_Log(DEBUG, "Started Net Log Module\n");
    return 0;
}

int NetLog_term() {
    debugNetFinish();
    return 0;
}