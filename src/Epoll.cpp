#include "Epoll.h"
#include "util.h"
#include "Server.h"
#include <string.h> 
#include <unistd.h>

#define MAX_EVENTS 1024

Epoll::Epoll() : epfd(epoll_create1(0)), events(new epoll_event[MAX_EVENTS]){
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

std::vector<Channel*> Epoll::poll(int timeout){
    std::vector<Channel*> clnt_ch;
    int nfds = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    //printf("%d\n",nfds);
    errif(nfds == -1, "epoll wait error");
    for(int i = 0; i < nfds; ++ i){
        Channel* ch = (Channel*)events[i].data.ptr;
        ch->SetRevent(events[i].events);
        clnt_ch.push_back(ch);
    }
    return clnt_ch;
}

void Epoll::updataChannel(Channel* ch){
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