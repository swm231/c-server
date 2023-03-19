#include "EventLoop.h"
#include "Socket.h"
#include "InetAddress.h"
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

void Server::NewConnection(Socket* serv_sock){
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    printf("new client fd: %d, IP: %s, Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    clnt_sock->setnonblocking();
    Channel* clnt_ch = new Channel(loop, clnt_sock->GetFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->GetFd());
    clnt_ch->SetCallback(cb);
    clnt_ch->enablereading();
}

void Server::handleReadEvent(int fd){
    char buf[MAX_BUFFER];
    while(true){
        bzero(&buf, sizeof buf);
        ssize_t read_bytes = read(fd, &buf, sizeof buf);
        if(read_bytes > 0){
            printf("message from fd %d: %s\n", fd, buf);
            for(int i = 0; i < read_bytes; ++i)
                buf[i] = toupper(buf[i]);
            write(fd, &buf, sizeof buf);
        }
        else if(read_bytes == -1 && errno == EINTR){
            printf("continue reading");
            continue;
        }
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if(read_bytes == 0){ 
            printf("EOF, client fd %d disconnected\n", fd);
            close(fd);
            break;
        }
    }
}