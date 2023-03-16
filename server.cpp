#include <sys/epoll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "util.h"

#define MAX_EVENT 1024
#define READ_BUFFER 1024

void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main(){
    int servfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(servfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    errif(bind(servfd, (sockaddr*)&serv_addr, sizeof serv_addr) == -1, "socket bind error");
    errif(listen(servfd, 128) == -1, "socket listen error");

    int epfd = epoll_create1(0);
    errif(epfd == -1, "epoll create error");

    struct epoll_event events[MAX_EVENT], ev;
    bzero(&events, sizeof events);

    bzero(&ev, sizeof ev);
    ev.data.fd = servfd;
    ev.events = EPOLLIN | EPOLLET;
    setnonblocking(servfd);
    epoll_ctl(epfd, EPOLL_CTL_ADD, servfd, &ev);

    while(true){
        int nfds = epoll_wait(epfd, events, MAX_EVENT, -1);
        errif(nfds == -1, "epoll wait error");
        for(int i = 0; i < nfds; ++ i){
            if(events[i].data.fd == servfd){
                struct sockaddr_in clnt_addr;
                bzero(&clnt_addr, sizeof clnt_addr);
                socklen_t clnt_addr_len = sizeof clnt_addr;

                int clntfd = accept(servfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
                errif(clntfd == -1, "client accept error");
                printf("new lient fd %d,IP: %s Port %d\n", clntfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));

                bzero(&ev, sizeof ev);
                ev.data.fd = clntfd;
                ev.events = EPOLLIN | EPOLLET;
                setnonblocking(clntfd);
                epoll_ctl(epfd, EPOLL_CTL_ADD, clntfd, &ev);
            }
            else if(events[i].events & EPOLLIN){
                char buf[READ_BUFFER];
                while(true){
                    bzero(&buf, sizeof buf);
                    ssize_t read_bytes = read(events[i].data.fd, buf, sizeof buf);
                    if(read_bytes > 0){
                        printf("message from fd %d: %s\n",events[i].data.fd, buf);
                        for(int i = 0; i < read_bytes; ++ i)
                                buf[i] = toupper(buf[i]);
                        write(events[i].data.fd, buf, sizeof buf);
                    }
                    else if(read_bytes == -1 && errno == EINTR){
                        printf("continue reading\n");
                        continue;
                    }
                    else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
                        printf("finish reading once, errno: %d\n", errno);
                        break;
                    }
                    else if(read_bytes == 0){
                        printf("EOF, client fd %d disconnect!\n", events[i].data.fd);
                        close(events[i].data.fd);
                        break;
                    }
                }
            }
            else{
                printf("other thing\n");
            }
        }
    }
}
