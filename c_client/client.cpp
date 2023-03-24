#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include "util.h"

#define BUFFER_SIZE 1024
void setnonblocking(int fd){
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}
int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serv_addr;
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("8.130.110.118");
    serv_addr.sin_port = htons(8888);
    errif(connect(sockfd, (sockaddr*)&serv_addr, sizeof(serv_addr)) == -1, "socket connect error");

    //setnonblocking(sockfd);
    char buf[BUFFER_SIZE];
    bzero(&buf, sizeof buf);
    ssize_t read_bytes = read(sockfd, buf, sizeof buf);
    buf[read_bytes] = '\0';
    printf("%s", buf);

    while(true){
        bzero(&buf, sizeof buf);
        scanf("%s", buf);
        ssize_t write_bytes = write(sockfd, &buf, strlen(buf));
        if(write_bytes == -1){
            printf("socket already disconnect");
            break;
        }

        bzero(&buf, sizeof buf);
        read_bytes = read(sockfd, buf, sizeof buf);
        buf[read_bytes]='\0';
        if(read_bytes > 0)
            printf("%s",buf);
        if(read_bytes == 0){
            printf("server socket disconnect!\n");
            close(sockfd);
            break;
        }
        else if(read_bytes == -1){
            close(sockfd);
            errif(true, "socket read error");
        }

    }
    close(sockfd);
    return 0;
}
