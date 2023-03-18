#include "Epoll.h"
#include "Socket.h"
#include "Channel.h"
#include "Server.h"
#include "InetAddress.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <ctype.h>

#define READ_BUFFER 1024

Server::Server(EventLoop* _loop) : loop(_loop){
    Socket* serv_sock = new Socket();
    InetAddress* serv_addr = new InetAddress(8888);
    serv_sock->setnonblocking();
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    Channel* serv_ch = new Channel(loop, serv_sock->GetFd());
    std::function<void()> cb = std::bind(&Server::NewConnection, this, serv_sock);
    serv_ch->SetCallback(cb);
    serv_ch->enablereading();
}

Server::~Server(){}

void Server::NewConnection(Socket* serv_sock){
    InetAddress* clnt_addr = new InetAddress();
    Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));
    printf("new client fd %d, IP %s, Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
    clnt_sock->setnonblocking();
    Channel* clnt_ch = new Channel(loop, clnt_sock->GetFd());
    std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->GetFd());
    clnt_ch->SetCallback(cb);
    clnt_ch->enablereading();
}

void Server::handleReadEvent(int sockfd){
    char buf[READ_BUFFER];
    bzero(&buf, sizeof buf);
    while(true){
        ssize_t read_bytes = read(sockfd, &buf, sizeof buf);
        if(read_bytes > 0){
            printf("message from fd %d: %s\n", sockfd, buf);
            for(int i = 0; i < read_bytes; ++ i)
                buf[i] = toupper(buf[i]);
            write(sockfd, &buf, sizeof buf);
        }
        else if(read_bytes == -1 && errno == EINTR){
            printf("read continue\n");
            continue;
        }
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish read once, errno: %d\n",errno);
            break;
        }
        else if(read_bytes == 0){
            printf("EOF, client fd %d disconnect!\n", sockfd);
            close(sockfd);
            break;
        }
    }
}