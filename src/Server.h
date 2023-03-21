#ifndef _SERVER_H_
#define _SERVER_H_

#include "EventLoop.h"
#include <map>

class Socket;
class Acceptor;
class Account;
class Connection;
class Mysql;
class Server{
private:
    EventLoop* MainReactor;
    Acceptor* acceptor;
    std::map<int, Connection*> connections;
    std::vector<EventLoop*> SubReators;
    ThreadPool* thread_pool;
    Mysql* mysql;
    std::function<void(Connection*)> OnConnectionCallback;
public:
    Server(EventLoop*);
    ~Server();

    void handleReadEvent(int);
    void NewConnection(Socket*);
    void DeleteConnection(Socket*);
    void OnConnect(std::function<void(Connection*)>);

    bool Insert(Account&&);
    bool Delete(Account&&);
    bool Modify(Account&&);
    bool Check(Account&&);
};

#endif