#ifndef _ACCPETOR_H_
#define _ACCPETOR_H_

#include <functional>

class EventLoop;
class Socket;
class InetAddress;
class Channel;
class Acceptor{
private:
    EventLoop* loop;
    Socket* sock;
    InetAddress* addr;
    Channel* ch;
public:
    Acceptor(EventLoop*);
    ~Acceptor();

    void AcceptConnection();
    std::function<void(Socket*)> NewConntectionCallback;
    void SetNewConnectionCallback(std::function<void(Socket*)>);
};

#endif