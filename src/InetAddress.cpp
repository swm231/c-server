#include "InetAddress.h"
#include "Socket.h"
#include <string.h>
#include <stddef.h>

InetAddress::InetAddress():len(sizeof addr){
    bzero(&addr, sizeof addr);
}

InetAddress::InetAddress(uint16_t _port) : len(sizeof addr){
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(_port);
    len = sizeof(addr);
}

InetAddress::~InetAddress(){}

void InetAddress::SetInetAddress(sockaddr_in _addr, socklen_t _len){
    addr = _addr;
    len = _len;
}

sockaddr_in InetAddress::GetAddr(){
    return addr;
}

socklen_t InetAddress::Getlen(){
    return len;
}