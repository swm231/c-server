#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <fcntl.h>
#include <unistd.h>

Socket::Socket() : fd(socket(AF_INET, SOCK_STREAM, 0)){
    errif(fd == -1, "socket create error");
}

Socket::Socket(int _fd) : fd(_fd){
    errif(fd == -1, "socket create error");
}

Socket::~Socket(){
    if(fd != -1){
        close(fd);
        fd = -1;
    }
}

void Socket::bind(InetAddress* addr){
    errif(::bind(fd, (sockaddr*)&addr->addr, addr->len) == -1, "socket bind error");
}

void Socket::listen(){
    errif(::listen(fd, 128) == -1, "socket listen error");
}

void Socket::setnonblocking(){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress* addr){
    int nfds = ::accept(fd, (sockaddr*)&addr->addr, &addr->len);
    errif(nfds == -1, "socket accpet error");
    return nfds;
}

int Socket::GetFd(){
    return fd;
}