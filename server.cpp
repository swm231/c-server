#include "Epoll.h"
#include "Socket.h"
#include "InetAddress.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <vector>
#include <ctype.h>

#define MAX_EVENT 1024
#define READ_BUFFER 1024

void handleReadEvent(int);

int main(){
    Socket* serv_sock = new Socket();
    InetAddress* serv_addr = new InetAddress(8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();

    Epoll* ep = new Epoll();
    serv_sock->setnonblocking();
    ep->AddFd(serv_sock->GetFd(), EPOLLIN | EPOLLET);

    while(true){
        std::vector<epoll_event> events = ep->poll();
        int nfds = events.size();
        for(int i = 0; i < nfds; i ++){
            if(events[i].data.fd == serv_sock->GetFd()){
                InetAddress* clnt_addr = new InetAddress();
                Socket* clnt_sock = new Socket(serv_sock->accept(clnt_addr));
                printf("new clinet fd %d, IP: %s, Port: %d\n", clnt_sock->GetFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setnonblocking();
                ep->AddFd(clnt_sock->GetFd(), EPOLLIN | EPOLLET);
            }
            else if(events[i].events & EPOLLIN){
                handleReadEvent(events[i].data.fd);
            }
            else {
                 printf("other thing\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd){
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
