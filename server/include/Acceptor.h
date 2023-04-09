#ifndef _ACCPETOR_H_
#define _ACCPETOR_H_

#include <functional>
#include <memory>

class EventLoop;
class Socket;
class Channel;
class Acceptor{
private:
    EventLoop* loop;

    std::unique_ptr<Socket> sock_;
    std::unique_ptr<Channel> ch_;
    std::function<void(std::shared_ptr<Socket>)> NewConntectionCallback;
public:
    Acceptor(EventLoop*);
    ~Acceptor();

    void AcceptConnection();
    void SetNewConnectionCallback(std::function<void(std::shared_ptr<Socket>)>);
};

#endif