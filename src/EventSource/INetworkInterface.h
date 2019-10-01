#ifndef INETWORK_INTERFACE_H
#define INETWORK_INTERFACE_H


/*
 * ## SocketInterface (The NetworkSource will have an instance of this and call methods on it)
 * Methods:
 * StartReceiving (this will also implicily *enable* events on the Source)
 * StopReceiving (this will also implicily *disable* events on the Source)
 * DataReadyToRead
 * DataReadyToWrite(? maybe)
 * getfd
 */

namespace VitaEventLoop
{
    

class NetworkInterface
{
public:

    virtual int StartSocket() = 0;

    virtual int StopSocket() = 0;

    virtual int InputEvent() = 0;

    virtual int OutpuEvent() = 0;

    virtual int GetFD() = 0;

private:
    
};


} //namespace VitaEventLoop
#endif // END INETWORK_INTERFACE_H
