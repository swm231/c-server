#ifndef _INETADDRESS_H_
#define _INETADDRESS_H_

#include <arpa/inet.h>
#include <memory>

class InetAddress{
private:
    struct sockaddr_in addr;
    socklen_t len;
public:
    InetAddress();
    InetAddress(uint16_t port);
    ~InetAddress();
    
    void SetInetAddress(sockaddr_in, socklen_t);
    sockaddr_in GetAddr();
    socklen_t Getlen();
};

#endif
