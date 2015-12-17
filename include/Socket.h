#ifndef SOCKET_H
#define SOCKET_H

#include "SockAddress.h"

class Socket_exception : public Net_exception { } ;

class Socket {
    SockAddress * sockAddr;
    int fd;
public:
    Socket(const SockAddress &sockAddr);

    Socket(const SockAddress &sockAddr, uint32_t timeout);

    int32_t read(size_t numBytes, void *dstBuf);

    int32_t write(size_t numBytes, const void *srcBuf);

    ~Socket();

private:
    void _construct(const SockAddress &sockAddr, uint32_t timeout);

    void _connect() ;

    void _assignAddress(const SockAddress &sockAddr);

    Socket(const Socket &o);

    Socket & operator=(const Socket &o);
};


#endif // SOCKET_H
