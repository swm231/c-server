#include "InetAddress.h"
#include <string.h>

InetAddress::InetAddress():len(sizeof addr){
    bzero(&addr, sizeof addr);
}

InetAddress::InetAddress(uint16_t port):len(sizeof addr){
    bzero(&addr, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress(){}
