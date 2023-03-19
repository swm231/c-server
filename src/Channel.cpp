#include "Channel.h"
#include "EventLoop.h"

Channel::Channel(EventLoop* _loop, int _fd) 
        : loop(_loop), fd(_fd), event(0), ready(0), inEpoll(false), useThreadPool(true){}

Channel::~Channel(){}

void Channel::HandleEvent(){
    if(ready & (EPOLLIN | EPOLLPRI)){
        if(useThreadPool)
            loop->AddTask(ReadCallback);
        else    
            ReadCallback();
    }
    if(ready & EPOLLOUT){
        if(useThreadPool)
            loop->AddTask(WriteCallback);
        else 
            WriteCallback();
    }
}

void Channel::enablereading(){
    event |= EPOLLIN | EPOLLPRI;
    loop->updataChannel(this);
}

void Channel::useET(){
    event |= EPOLLET;
    loop->updataChannel(this);
}

int Channel::GetFd(){
    return fd;
}

uint32_t Channel::GetEvent(){
    return event;
}

uint32_t Channel::GetReady(){
    return ready;
}

bool Channel::GetInEpoll(){
    return inEpoll;
}

void Channel::SetInEpoll(bool _in){
    inEpoll = _in;
}

void Channel::SetReady(uint32_t t){
    ready = t;
}

void Channel::SetReadCallback(std::function<void()> cb){
    ReadCallback = cb;
}

void Channel::SetUseThreadPool(bool bl){
    useThreadPool = bl;
}