#ifndef EVENTLOOP_MISC_H
#define EVENTLOOP_MISC_H

namespace VitaEventLoop
{
    typedef struct vita_uv_buf {
        void* base;
        int len;
    } vita_uv_buf;
} // namespace VitaEventLoop


#endif