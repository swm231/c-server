#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include "Epoll.h"

class Channel;
class Epoll;
class EventLoop{
private:
    Epoll* ep;
    bool quit;
public:
    EventLoop();
    ~EventLoop();
    void loop();
    void updataChannel(Channel*);
};

#endif