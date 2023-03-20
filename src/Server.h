#ifndef _SERVER_H_
#define _SERVER_H_

#include "EventLoop.h"
#include <map>

class Socket;
class Acceptor;
class Connection;
class Server{
private:
    EventLoop* MainReactor;
    Acceptor* acceptor;
    std::map<int, Connection*> connections;
    std::vector<EventLoop*> SubReators;
    ThreadPool* thread_pool;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void NewConnection(Socket*);
    void DeleteConnection(Socket*);
};

#endif