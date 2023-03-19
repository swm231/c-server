#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>

class EventLoop;
class Socket;
class Channel;
class Connection{
private:
    EventLoop* loop;
    Socket* sock;
    Channel* ch;
    std::function<void(Socket*)> DeleteConnectionCallback;
public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo(int);
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
};

#endif