#ifndef _ACCPETOR_H_
#define _ACCPETOR_H_

#include <functional>

class EventLoop;
class Socket;
class Channel;
class Acceptor{
private:
    EventLoop* loop;
    Socket* sock;
    Channel* ch;
    std::function<void(Socket*)> NewConntectionCallback;
public:
    Acceptor(EventLoop*);
    ~Acceptor();

    void AcceptConnection();
    void SetNewConnectionCallback(std::function<void(Socket*)>);
};

#endif