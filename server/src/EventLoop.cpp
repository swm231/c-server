#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadCache.h"
#include <vector>

EventLoop::EventLoop() :
    quit(false), ep(new (ThreadCache::operator new(sizeof(Epoll))) Epoll()){}

EventLoop::~EventLoop(){
    ThreadCache::operator delete (ep, sizeof(*ep));
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

void EventLoop::deleteChannel(Channel* ch){
    ep->deleteChannel(ch);
}

void EventLoop::AddTask(std::function<void()> func){
    func();
}
