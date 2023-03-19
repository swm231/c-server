#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop){
    sock = new Socket();
    addr = new InetAddress(8888);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    ch = new Channel(loop, sock->GetFd());
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    ch->SetCallback(cb);
    ch->enablereading();
}

Acceptor::~Acceptor(){
    delete sock;
    delete addr;
    delete ch;
}

void Acceptor::AcceptConnection(){
    NewConntectionCallback(sock);
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket*)> _cb){
    NewConntectionCallback = _cb;
}