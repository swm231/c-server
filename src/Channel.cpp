#include "Channel.h"
#include "Epoll.h"

Channel::Channel(Epoll* _ep, int _fd) : ep(_ep), fd(_fd), event(0), revent(0), inEpoll(false){}

Channel::~Channel(){
}

void Channel::enablereading(){
    event = EPOLLIN | EPOLLET;
    ep->updataChannel(this);
}

int Channel::GetFd(){
    return fd;
}

uint32_t Channel::GetEvent(){
    return event;
}

uint32_t Channel::GetRevent(){
    return revent;
}

bool Channel::GetStatus(){
    return inEpoll;
}

void Channel::SetStatus(){
    inEpoll = true;
}

void Channel::SetRevent(uint32_t ev){
    revent = ev;
}