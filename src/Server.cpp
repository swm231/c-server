#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Connection.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Server.h"
#include "Mysql.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Server::Server(EventLoop* _loop) : MainReactor(_loop), acceptor(new Acceptor(MainReactor)), mysql(new Mysql()){
    mysql->InitSql("localhost","root","SZG123456szg","test");
    
    std::function<void(Socket*)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor->SetNewConnectionCallback(cb);
    
    int size = std::thread::hardware_concurrency();
    thread_pool = new ThreadPool(size);
    for(int i = 0; i < size; ++ i)
        SubReators.push_back(new EventLoop());

    for(int i = 0; i < size; ++ i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, SubReators[i]);
        thread_pool->add(sub_loop);
    }
}

Server::~Server(){
    delete acceptor;
    delete thread_pool;
    for(int i = 0; i < SubReators.size(); ++ i)
        delete SubReators[i];
}

void Server::NewConnection(Socket* sock){   //新连接第二个函数 新建connection 分配工作线程
    // 全随机调度
    int rd = sock->GetFd() % SubReators.size();
    Connection* conn = new Connection(this, SubReators[rd], sock); 
    std::function<void(Socket*)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    //conn->SetOnConnectionCallback(OnConnectionCallback);  //为新客服端channel的套接字绑定 函数
    connections[sock->GetFd()] = conn;
}

void Server::DeleteConnection(Socket* sock){
    Connection* conn = connections[sock->GetFd()];
    connections.erase(sock->GetFd());
    delete conn;
}

void Server::OnConnect(std::function<void(Connection*)> cb){
    OnConnectionCallback = std::move(cb);
}

bool Server::Insert(const Account* acc){
    return mysql->Insert(acc);
}

bool Server::Delete(const Account* acc){
    return mysql->Delete(acc);
}

bool Server::Modify(const Account* acc){
    return mysql->Modify(acc);
}

ssize_t Server::Check(const Account* acc){
    return mysql->Check(acc);
}