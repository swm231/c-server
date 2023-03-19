#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>

class EventLoop;
class Socket;
class Buffer;
class Channel;
class Connection{
private:
    EventLoop* loop;
    Socket* sock;
    Channel* ch;
    Buffer* ReadBuffer;
    std::function<void(Socket*)> DeleteConnectionCallback;
    std::string *inBuffer;

public:
    Connection(EventLoop*, Socket*);
    ~Connection();

    void echo(int);
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
    void send(int);
};

#endif