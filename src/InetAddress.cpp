#include "InetAddress.h"
#include "Socket.h"
#include <string.h>

InetAddress::InetAddress():len(sizeof addr){
    bzero(&addr, sizeof addr);
}

InetAddress::InetAddress(uint16_t _port) : len(sizoef addr){
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(_port);
}

InetAddress::~InetAddress(){}