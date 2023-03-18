#include "EventLoop.h"
#include <vector>

EventLoop::EventLoop():ep(new Epoll()), quit(false){}

EventLoop::~EventLoop(){
    delete ep;
}

void EventLoop::loop(){
    while(!quit){
        std::vector<Channel*> chs;
        chs = ep->poll(-1);
        for(auto it = chs.begin(); it != chs.end(); ++ it){
            (*it)->HandleEvent();
        }
    }
}

void EventLoop::updataChannel(Channel* ch){
    ep->updataChannel(ch);
}