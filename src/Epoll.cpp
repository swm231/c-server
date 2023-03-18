#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <string.h>

#define MAX_EVENTS 1024

Epoll::Epoll():epfd(epoll_create1(0)),events(new epoll_event[MAX_EVENTS]){
    errif(epfd == -1, "epoll create error");
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll(){
    if(epfd != -1){
        close(epfd);
        epfd = -1;
    }
    delete [] events;
}

void Epoll::AddFd(int fd, uint32_t status){
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.data.fd = fd;
    ev.events = status;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
}

/*
std::vector<epoll_event> Epoll::poll(int timeout){
    std::vector<epoll_event> Active_Events;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++ i)
        Active_Events.push_back(events[i]);
    return Active_Events;
}
*/

std::vector<Channel*> Epoll::poll(int timeout){
    std::vector<Channel*> Channel_Events;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++ i){
        Channel* ch = (Channel*)events[i].data.ptr;
        ch->SetRevent(events[i].events);
        Channel_Events.push_back(ch);
    }
    return Channel_Events;
}

void Epoll::updataChannel(Channel* ch)
{
    int fd = ch->GetFd();
    struct epoll_event ev;
    bzero(&ev, sizeof ev);
    ev.data.ptr = ch;
    ev.events = ch->GetEvent();
    if(!ch->GetStatus()){
        errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "epoll add error");
        ch->SetStatus();
    }
    else{
        errif(epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &ev) == -1, "epoll mod error");
    }
}