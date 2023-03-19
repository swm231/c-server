#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) : loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd())){
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->GetFd());
    ch->SetCallback(cb);
    ch->enablereading();
    ReadBffer = new Buffer();
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
        if(read_bytes > 0)
            ReadBffer->append(buf, read_bytes);
        else if(read_bytes == -1 && errno == EINTR){
            printf("continue reading");
            continue;
        }
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish reading once");
            printf("message from fd %d, %s\n", fd, ReadBffer->c_str());
            errif(write(fd, ReadBffer->c_str(), ReadBffer->size()) == -1, "socket write error");
            ReadBffer->clear();
            break;
        }
        else if(read_bytes == 0){ 
            printf("EOF, client fd %d disconnected\n", fd);
            close(fd);
            DeleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> cb){
    DeleteConnectionCallback = cb;
}