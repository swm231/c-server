#include "EventLoop.h"
#include "Server.h"
#include "Mysql.h"
#include "Channel.h"
#include "ThreadPool.h"
#include "Connection.h"
#include "Socket.h"
#include "ThreadCache.h"
#include <string>
#include <string.h>
#include <iostream>
//g++ *.cpp -o main -lpthread `mysql_config --cflags --libs`

int main(){
    EventLoop* loop = new (ThreadCache::operator new(sizeof(EventLoop))) EventLoop();
    Server* server = new Server(loop);

    loop->loop();

    ThreadCache::operator delete (loop, sizeof(*loop));
    delete server;
    return 0;
}