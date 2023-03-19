#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) : loop(_loop), fd(_fd), event(0), revent(0), inEpoll(false){}

Channel::~Channel(){}

void Channel::enablereading(){
    event |= EPOLLIN | EPOLLET;
    loop->updataChannel(this);
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

void Channel::SetRevent(uint32_t t){
    revent = t;
}

void Channel::SetCallback(std::function<void()> cb){
    callback = cb;
}

void Channel::HandleEvent(){
    callback();
}