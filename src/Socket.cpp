#include "Socket.h"
#include <cstring>

Socket::Socket(const SockAddress &sockAddr) : sockAddr(0), fd(-1) {
    _construct(sockAddr, 0);
}

Socket::Socket(const SockAddress &sockAddr, uint32_t timeout) : sockAddr(0), fd(-1) {
    _construct(sockAddr, timeout);
}

Socket::Socket(const Socket &o) : fd(o.fd) {
    _assignAddress(*o.sockAddr);
}

Socket & Socket::operator=(const Socket &o) {
    this->fd = o.fd;
    _assignAddress(*o.sockAddr);
    return *this;
}

Socket::~Socket() {
    close(fd);
    delete sockAddr;
}

int32_t Socket::read(size_t numBytes, void *dstBuf) {
    int32_t szRead,
            offset = 0,
            toRead = numBytes;
    uint8_t *buf = (uint8_t *)dstBuf;
    while((szRead=recv(fd, &buf[offset], toRead, 0)) > 0 && toRead > 0) {
        offset+=szRead;
        toRead-=szRead;
    }
    if(szRead == -1) {
        throw Socket_exception();
    }
    return numBytes-szRead;
}

int32_t Socket::write(size_t numBytes, const void *srcBuf) {
    int32_t szSent,
            offset = 0,
            toSend = numBytes;
    const uint8_t *buf = (const uint8_t *)srcBuf;
    while((szSent=send(fd, &buf[offset], toSend, 0)) > 0) {
        offset+=szSent;
        toSend-=szSent;
    }
    if(szSent==-1) {
        throw Socket_exception();
    }
    return numBytes-szSent;
}

void Socket::_connect() {
    int res = -1;
    if(sockAddr->ipversion() == SockAddress::V4) {
        sockaddr_in addr_in;
        addr_in.sin_port = sockAddr->port();
        addr_in.sin_family = AF_INET;
        addr_in.sin_addr.s_addr = *sockAddr->binaddr();
        res = connect(fd, (const sockaddr*)&addr_in, sizeof(addr_in));
    } else if (sockAddr->ipversion() == SockAddress::V6) {
        sockaddr_in6 addr_in6;
        addr_in6.sin6_family = AF_INET6;
        addr_in6.sin6_port = sockAddr->port();
        addr_in6.sin6_flowinfo = 0;
        memcpy(addr_in6.sin6_addr.s6_addr, sockAddr->binaddr(), 16);
        addr_in6.sin6_scope_id = 0;
        res = connect(fd, (const sockaddr*)&addr_in6, sizeof(addr_in6));
    }
    if(res == -1) {
        throw Socket_exception();
    }
}

void Socket::_assignAddress(const SockAddress &sockAddr) {
    if(sockAddr.ipversion() == SockAddress::V4) {
        this->sockAddr = new Sock4Address(dynamic_cast<const Sock4Address&>(sockAddr));
    } else if (sockAddr.ipversion() == SockAddress::V6) {
        this->sockAddr = new Sock6Address(dynamic_cast<const Sock6Address&>(sockAddr));
    } else {
        throw Socket_exception();
    }
}

void Socket::_construct(const SockAddress &sockAddr, uint32_t timeout) {

    _assignAddress(sockAddr);

    int domain = -1,
        type = SOCK_STREAM;

    if(sockAddr.ipversion() == SockAddress::V4) {
        domain = AF_INET;
    } else if (sockAddr.ipversion() == SockAddress::V6) {
        domain = AF_INET6;
    } else {
        throw Socket_exception();
    }

    if((fd=socket(domain, type, 0))==-1) {
        delete this->sockAddr;
        throw Socket_exception();
    }

    if(timeout != 0) {
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = timeout*1000;
        if(setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(timeval))==-1) {
            close(fd);
            delete this->sockAddr;
            throw Socket_exception();
        }
        if(setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(timeval))==-1) {
            close(fd);
            delete this->sockAddr;
            throw Socket_exception();
        }
    }

    try {
        _connect();
    } catch (const Socket_exception &error) {
        delete this->sockAddr;
        close(fd);
        throw;
    }
}
