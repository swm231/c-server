#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Server.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(Server* _server, EventLoop* _loop, Socket* _sock) 
        : server(_server), loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd()))
        ,ReadBuffer(new Buffer()), SendBuffer(new Buffer()), state(State::Invalid){
    
    state = State::Connected;
    ch->enablereading();
    ch->useET();
}

Connection::~Connection(){
    delete ch;
    delete sock;
    delete ReadBuffer;
    delete SendBuffer;
}

void Connection::Send(int fd){
    char buf[SendBuffer->size()];
    strcpy(buf, SendBuffer->c_str());
    int data_size = SendBuffer->size();
    int data_left = data_size;
    while(data_left > 0){
        ssize_t write_bytes = write(fd, buf + data_size - data_left, data_left);
        if(write_bytes == -1 && errno == EAGAIN)
            break;
        data_left -= write_bytes;
    }
    SendBuffer->clear();
}

void Connection::Read(){
    ReadBuffer->clear();
    char buf[MAX_BUFFER];
    bzero(&buf, sizeof buf);
    ssize_t read_bytes = read(sock->GetFd(), buf, sizeof buf);
    if(read_bytes == 0){
        printf("EOF, client fd %d disconnected\n", sock->GetFd());
        state = State::DisConnected;
        loop->deleteChannel(ch);
        close(sock->GetFd());
    }
    ReadBuffer->append(buf, read_bytes);
}

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> cb){
    DeleteConnectionCallback = cb;
}

void Connection::SetOnConnectionCallback(std::function<void(Connection*)> cb){
    OnConnectionCallback = cb;
    ch->SetReadCallback([this](){OnConnectionCallback(this);});
}

void Connection::Close(){
    DeleteConnectionCallback(sock);
}

State Connection::GetState(){
    return state;
}

bool Connection::Insert(Account&& acc){
    return server->Insert(std::move(acc));
}

bool Connection::Delete(Account&& acc){
    return server->Delete(std::move(acc));
}

bool Connection::Modify(Account&& acc){
    return server->Modify(std::move(acc));
}

bool Connection::Check(Account&& acc){
    return server->Check(std::move(acc));
}