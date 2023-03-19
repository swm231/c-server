#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop){
    sock = new Socket();
    InetAddress* addr = new InetAddress(8888);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    ch = new Channel(loop, sock->GetFd());
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    ch->SetCallback(cb);
    ch->enablereading();
    delete addr;
}

Acceptor::~Acceptor(){
    delete sock;
    delete ch;
}

void Acceptor::AcceptConnection(){
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(sock->accept(clnt_addr));
    printf("new client fd: %d, IP: %s, Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->GetAddr().sin_addr), ntohs(clnt_addr->GetAddr().sin_port));
    clnt_sock->setnonblocking();
    NewConntectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::SetNewConnectionCallback(std::function<void(Socket*)> _cb){
    NewConntectionCallback = _cb;
}