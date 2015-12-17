#ifndef INETADDRESS_H
#define INETADDRESS_H

#include <string>
#include <arpa/inet.h>
#include <netinet/in.h>


class Net_exception {
};

class SockAddress {
public:
    enum AddressType { V4 = 4, V6 = 6 };
protected:

    AddressType type;
    mutable std::string addrRep;

    static void toStringRep(const uint32_t *addr,
                            AddressType at,
                            const size_t ipStringSize,
                            std::string &dest);

public:
    SockAddress(AddressType type) ;
    SockAddress(AddressType type, const std::string &addrRep) ;
    virtual const uint16_t port() const = 0;
    virtual const std::string& ipaddress() const = 0;
    virtual const uint32_t* binaddr() const = 0;
    virtual AddressType ipversion() const {
        return type;
    }
    virtual ~SockAddress();
};

class Sock4Address : public SockAddress {
    static const size_t INET_ADDRESS_STRING_LEN = INET_ADDRSTRLEN;
    uint32_t addr;
    uint16_t p;
public:

    Sock4Address(uint32_t addr, uint16_t port);

    Sock4Address(const std::string &addr, uint16_t port);

    Sock4Address(const Sock4Address &o);

    Sock4Address operator=(const Sock4Address &o);

    const std::string& ipaddress() const;

    const uint16_t port() const;

    const uint32_t* binaddr() const;

};

class Sock6Address : public SockAddress {
    static const size_t INET_ADDRESS_STRING_LEN = INET6_ADDRSTRLEN;
    uint32_t addr[4];
    uint16_t p;
public:

    Sock6Address(const uint32_t addr[4], uint16_t port);

    Sock6Address(const std::string &addr, uint16_t port);

    Sock6Address(const Sock6Address &o);

    Sock6Address operator=(const Sock6Address &o);

    const uint16_t port() const;

    const uint32_t * binaddr() const;

    const std::string& ipaddress() const;

};


#endif // INETADDRESS_H
