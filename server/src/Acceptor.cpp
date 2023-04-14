#include "Acceptor.h"
#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "InetAddress.h"
#include "ThreadCache.h"

Acceptor::Acceptor(EventLoop* _loop) : loop(_loop){
    sock_ = std::make_unique<Socket>();
    InetAddress* addr = new(ThreadCache::operator new(sizeof(InetAddress))) InetAddress(8888);
    sock_->bind(addr);
    sock_->listen();
    sock_->setnonblocking();
    ch_ = std::make_unique<Channel>(loop, sock_->GetFd());
    std::function<void()> cb = std::bind(&Acceptor::AcceptConnection, this);
    ch_->SetReadCallback(cb);
    ch_->enablereading();
    ThreadCache::operator delete(addr, sizeof(*addr));
}

Acceptor::~Acceptor(){}

void Acceptor::AcceptConnection(){  //新连接第一个函数 获取套接字
    InetAddress* clnt_addr = new (ThreadCache::operator new (sizeof(InetAddress))) InetAddress();
    std::shared_ptr<Socket> clnt_sock = std::make_shared<Socket>(sock_->accept(clnt_addr));
    printf("new client fd: %d, IP: %s, Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->GetAddr().sin_addr), ntohs(clnt_addr->GetAddr().sin_port));
    clnt_sock->setnonblocking();
    NewConntectionCallback(clnt_sock);
    ThreadCache::operator delete (clnt_addr, sizeof(*clnt_addr));
}

void Acceptor::SetNewConnectionCallback(std::function<void(std::shared_ptr<Socket>)> _cb){
    NewConntectionCallback = _cb;
}