#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include <vector>

EventLoop::EventLoop() : ep(new Epoll()), quit(false), pool(new ThreadPool){}

EventLoop::~EventLoop(){
    delete ep;
}

void EventLoop::loop(){
    while(!quit){
        std::vector<Channel*> ch;
        ch = ep->poll();
        for(auto it = ch.begin(); it != ch.end(); ++ it)
            (*it)->HandleEvent();
    }
}

void EventLoop::updataChannel(Channel* ch){
    ep->updataChannel(ch);
}

void EventLoop::AddTask(std::function<void()> func){
    pool->add(func);
}