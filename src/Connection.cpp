#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd())){
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->GetFd());
    ch->SetCallback(cb);
    ch->enablereading();
}

Connection::~Connection(){
    delete ch;
    delete sock;
}

void Connection::echo(int fd){
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

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> cb){
    DeleteConnectionCallback = cb;
}