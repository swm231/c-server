#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Connection.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Server.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Server::Server(EventLoop* _loop) : loop(_loop){
    acceptor = new Acceptor(loop);
    std::function<void(Socket*)> cb = std::bind(&Server::NewConnection, this, std::placeholders::_1);
    acceptor->SetNewConnectionCallback(cb);
}

Server::~Server(){
    delete acceptor;
}

void Server::NewConnection(Socket* sock){
    Connection* conn = new Connection(loop, sock);
    std::function<void(Socket*)> cb = std::bind(&Server::DeleteConnection, this, std::placeholders::_1);
    conn->SetDeleteConnectionCallback(cb);
    connections[sock->GetFd()] = conn;
}

void Server::DeleteConnection(Socket* sock){
    Connection* conn = connections[sock->GetFd()];
    connections.erase(sock->GetFd());
    delete conn;
}