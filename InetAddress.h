#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include <arpa/inet.h>

class InetAddress{
public:
    struct sockaddr_in addr;
    socklen_t len;
    InetAddress();
    InetAddress(uint16_t port);
    ~InetAddress();
};

#endif
