#ifndef EVENTLOOP_MISC_H
#define EVENTLOOP_MISC_H

namespace VitaEventLoop
{
    typedef struct vita_uv_buf {
        char* base;
        int len;
    } vita_uv_buf;
} // namespace VitaEventLoop


#endif