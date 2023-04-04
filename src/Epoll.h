#ifndef _EPOLL_H_
#define _EPOLL_H_

#include <vector>
#include <sys/epoll.h>
#include <memory>
#include "Channel.h"

class Epoll{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();

    void AddFd(int fd, uint32_t status);
    std::vector<Channel*> poll(int timeout = -1);
    void updataChannel(Channel*);
    void deleteChannel(Channel*);
};


#endif
