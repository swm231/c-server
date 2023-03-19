#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Channel.h"
#include "EventLoop.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(EventLoop* _loop, Socket* _sock) 
        : loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd())), inBuffer(new std::string()), ReadBuffer(new Buffer()){
    ch->enablereading();
    ch->useET();
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->GetFd());
    ch->SetReadCallback(cb);
    ch->SetUseThreadPool(true);
}

Connection::~Connection(){
    delete ch;
    delete sock;
    delete ReadBuffer;
}

void Connection::echo(int fd){
    char buf[MAX_BUFFER];
    while(true){
        bzero(&buf, sizeof buf);
        ssize_t read_bytes = read(fd, &buf, sizeof buf);
        if(read_bytes > 0)
            ReadBuffer->append(buf, read_bytes);
        else if(read_bytes == -1 && errno == EINTR){
            printf("continue reading");
            continue;
        }
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){
            printf("finish reading once");
            printf("message from fd %d, %s\n", fd, ReadBuffer->c_str());
            send(fd);
            ReadBuffer->clear();
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

void Connection::send(int fd){
    char buf[ReadBuffer->size()];
    strcpy(buf, ReadBuffer->c_str());
    int data_size = ReadBuffer->size();
    for(int i = 0; i < data_size; ++ i)
        buf[i] = toupper(buf[i]);
    int data_left = data_size;
    while(data_left > 0){
        ssize_t write_bytes = write(fd, buf + data_size - data_left, data_left);
        if(write_bytes == -1 && errno == EAGAIN)
            break;
        data_left -= write_bytes;
    }
}