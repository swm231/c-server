#include "EventLoop.h"
#include "Server.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <string>
#include <iostream>

int main(){
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);
    loop->loop();

    return 0;
}