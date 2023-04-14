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

Server::Server(EventLoop* _loop) : MainReactor(_loop),
        acceptor(new (ThreadCache::operator new(sizeof(Acceptor))) Acceptor(MainReactor)),
            mysql(new (ThreadCache::operator new(sizeof(Mysql))) Mysql()){
    mysql->InitSql("localhost","root","SZG123456szg","test");
    
    std::function<void(std::shared_ptr<Socket>)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor->SetNewConnectionCallback(cb);
    
    int size = std::thread::hardware_concurrency();
    thread_pool = new (ThreadCache::operator new(sizeof(ThreadPool))) ThreadPool(size);
    for(int i = 0; i < size; ++ i)
        SubReators.push_back(new (ThreadCache::operator new(sizeof(EventLoop))) EventLoop());
    for(int i = 0; i < size; ++ i){
        std::function<void()> sub_loop = std::bind(&EventLoop::loop, SubReators[i]);
        thread_pool->add(sub_loop);
    }
}

Server::~Server(){
    ThreadCache::operator delete (acceptor, sizeof(*acceptor));
    ThreadCache::operator delete (mysql, sizeof(*mysql));
    ThreadCache::operator delete (thread_pool, sizeof(*thread_pool));
    for(int i = 0; i < SubReators.size(); ++ i)
        ThreadCache::operator delete (SubReators[i], sizeof(*SubReators[i]));
}

void Server::NewConnection(std::shared_ptr<Socket> sock){   //新连接第二个函数 新建connection 分配工作线程
    // 全随机调度
    int rd = sock->GetFd() % SubReators.size();
    Connection* conn = new (ThreadCache::operator new (sizeof(Connection))) Connection(this, SubReators[rd], sock);
    std::function<void(std::shared_ptr<Socket>)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    connections[sock->GetFd()] = conn;
}

void Server::DeleteConnection(std::shared_ptr<Socket> sock){
    Connection* conn = connections[sock->GetFd()];
    connections.erase(sock->GetFd());
    ThreadCache::operator delete (conn, sizeof(*conn));
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

bool Server::FdSet(const Account* acc, int fd){
    return mysql->Fd_Modify(acc, fd);
}

std::vector<std::string> Server::LookList(const Account* acc){
    return mysql->LookList(acc);
}

std::vector<int> Server::GetOnlFd(){
    return mysql->GetOnlFd();
}

bool Server::Find(const char* str){
    return mysql->Find(str);
}

void Server::AddShip(Account* acc, const char* str){
    mysql->AddShip(acc, str);
}

int Server::FindFd(const char* str){
    return mysql->FindFd(str);
}