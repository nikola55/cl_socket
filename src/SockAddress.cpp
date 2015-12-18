#include "SockAddress.h"
#include <arpa/inet.h>
#include <netinet/in.h>

SockAddress::SockAddress(AddressType type):
        type(type) {};

SockAddress::SockAddress(AddressType type, const std::string &addrRep) :
    type(type), addrRep(addrRep) {
}

void SockAddress::toStringRep(const uint32_t *addr,
                              AddressType at,
                              const size_t ipStringSize,
                              std::string &dest) {

    char repCStr[ipStringSize];
    uint32_t afType;

    if(at == V4) {
        afType = AF_INET;
    } else if (at == V6) {
        afType = AF_INET6;
    } else {
        throw Net_exception();
    }

    if(inet_ntop(afType, addr, repCStr, ipStringSize) == NULL) {
        throw Net_exception();
    }

    dest = repCStr;
}

SockAddress::~SockAddress() {

}

Sock4Address::Sock4Address(uint32_t addr, uint16_t port) :
    SockAddress(SockAddress::V4), addr(htonl(addr)), p(htons(port)) {
}

Sock4Address::Sock4Address(const std::string &addr, uint16_t port) : SockAddress(SockAddress::V4, addr) {
    if(inet_pton(AF_INET, addr.c_str(), &this->addr) != 1) {
        throw Net_exception();
    }
    this->p = htons(port);
}

Sock4Address::Sock4Address(const Sock4Address &o) :
    SockAddress(SockAddress::V4, o.addrRep), addr(o.addr), p(o.p) {
}

Sock4Address Sock4Address::operator=(const Sock4Address &o) {
    this->type = o.type;
    this->addrRep = o.addrRep;
    this->addr = o.addr;
    this->p = o.p;
    return *this;
}

const std::string& Sock4Address::ipaddress() const {
    if(addrRep=="") {
        toStringRep(&addr, type, INET_ADDRESS_STRING_LEN*sizeof(char), addrRep);
    }
    return addrRep;
}

const uint16_t Sock4Address::port() const {
    return ntohs(p);
}

const uint32_t* Sock4Address::binaddr() const {
    return &addr;
}

const uint16_t Sock4Address::binport() const {
    return p;
}

Sock6Address::Sock6Address(const uint32_t addr[4], uint16_t port) : SockAddress(SockAddress::V6) {
    this->addr[0] = htonl(addr[0]);
    this->addr[1] = htonl(addr[1]);
    this->addr[2] = htonl(addr[2]);
    this->addr[3] = htonl(addr[3]);
    this->p = htons(port);
}

Sock6Address::Sock6Address(const std::string &addr, uint16_t port) : SockAddress(SockAddress::V6, addr) {
    if(inet_pton(AF_INET6, addr.c_str(), this->addr) != 1) {
        throw Net_exception();
    }
    this->p = htons(port);
}

Sock6Address::Sock6Address(const Sock6Address &o) : SockAddress(SockAddress::V6, o.addrRep), p(o.p) {
    this->addr[0] = o.addr[0];
    this->addr[1] = o.addr[1];
    this->addr[2] = o.addr[2];
    this->addr[3] = o.addr[3];
}

Sock6Address Sock6Address::operator=(const Sock6Address &o) {
    this->addrRep = o.addrRep;
    this->type = o.type;
    this->addr[0] = o.addr[0];
    this->addr[1] = o.addr[1];
    this->addr[2] = o.addr[2];
    this->addr[3] = o.addr[3];
    this->p = o.p;
    return *this;
}

const uint16_t Sock6Address::port() const {
    return ntohs(p);
}

const uint32_t * Sock6Address::binaddr() const {
    return addr;
}

const uint16_t Sock6Address::binport() const {
    return p;
}

const std::string& Sock6Address::ipaddress() const {
    if(addrRep=="") {
        toStringRep(addr, type, INET_ADDRESS_STRING_LEN*sizeof(char), addrRep);
    }
    return addrRep;
}
