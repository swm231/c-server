#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "util.h"

int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    errif(sockfd == -1, "socket create error");

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(8888);

    errif(bind(sockfd, (const sockaddr*)&serv_addr, sizeof serv_addr) == -1, "socket bind error");
    errif(listen(sockfd, 128) == -1, "socket listen errror");

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_len = sizeof clnt_addr;
    bzero(&clnt_addr, sizeof clnt_addr);
    int clntfd = accept(sockfd, (sockaddr*)&clnt_addr, &clnt_addr_len);
    errif(clntfd == -1, "client accept error");

    printf("new client fd %d,IP: %s Port: %d\n", clntfd, inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
    while(true){
        char buf[1024];
        bzero(&buf, sizeof buf);
        ssize_t read_bytes = read(clntfd, &buf, sizeof buf);
        if(read_bytes > 0){
            printf("accept message %s from IP:%s\n", buf, inet_ntoa(clnt_addr.sin_addr));
            for(int i = 0; i < read_bytes; ++ i)
                buf[i] = toupper(buf[i]);
            errif(write(clntfd, &buf, sizeof buf) == -1, "clint write error");
        }
        else if(read_bytes == 0){
            printf("clint %d disconnect!\n", clntfd);
            close(clntfd);
            break;
        }
        else if(read_bytes == -1){
            close(clntfd);
            errif(true, "clint read error");
        }
    }
    close(sockfd);
    return 0;
}
