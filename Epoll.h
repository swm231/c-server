#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <vector>
#include <sys/epoll.h>

class Epoll{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();

    void AddFd(int fd, uint32_t status);
    std::vector<epoll_event> poll(int timeout = -1);
};


#endif