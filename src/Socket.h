#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <sys/socket.h>
#include "InetAddress.h"

class Socket{
private:
    int fd;

public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress*);
    void listen();
    void setnonblocking();

    int accept(InetAddress*);
    int GetFd();
};


#endif
