#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Mysql.h"
#include "Online.h"
#include "Channel.h"
#include "Login.h"
#include "EventLoop.h"
#include "Server.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(Server* _server, EventLoop* _loop, Socket* _sock) 
        : server(_server), loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd())),
          acc(new Account()),ReadBuffer(new Buffer()), SendBuffer(new Buffer()), 
          state(State::Invalid), login(new LogIn(this, ch, sock)) {
    onl = new Online(this);
    Send_str("Successfully connecting to the server\n 1 Sign in\n 2 Sign on\n");

}

Connection::~Connection(){
    delete sock;
    delete ch;
    delete acc;
    delete ReadBuffer;
    delete SendBuffer;
    delete login;
}

void Connection::Send_str(const char* str){
    SendBuffer->clear();
    SendBuffer->SetBuf(str);
    Send();
}

void Connection::Send(){
    int fd = sock->GetFd();
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
    while(true){
        bzero(&buf, sizeof buf);
        ssize_t read_bytes = read(sock->GetFd(), buf, sizeof buf);
        if(read_bytes > 0){
            ReadBuffer->append(buf, read_bytes);
        }
        else if(read_bytes == -1 && errno == EINTR)
            continue;
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            break;
        else if(read_bytes == 0){
            printf("EOF, client fd %d disconnection\n", sock->GetFd());
            //do task
            FdSet(-1);
            state = State::Closed;
            close(sock->GetFd());
            break;
        }
    }
}

void Connection::sBuf_append(const char* str, int sz){
    SendBuffer->append(str, sz);
}

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> cb){
    DeleteConnectionCallback = cb;
}

void Connection::SetOnLineCallback(std::function<void()> cb){
    OnLineCallback = std::move(cb);
}

void Connection::Set_Online_channel(){
    ch->SetReadCallback(std::move(OnLineCallback));
    loop->updataChannel(ch);
}

const char* Connection::Readp(){
    return ReadBuffer->c_str();
}

void Connection::Set_Name(){
    acc->Set_Name(ReadBuffer->c_str());
}

void Connection::Set_Pass(){
    acc->Set_Pass(ReadBuffer->c_str());
}

void Connection::Set_State(State st){
    state = st;
}

void Connection::Close(){
    DeleteConnectionCallback(sock);
}

State Connection::GetState(){
    return state;
}

bool Connection::Insert(){
    return server->Insert(acc);
}

bool Connection::Delete(){
    return server->Delete(acc);
}

bool Connection::Modify(){
    return server->Modify(acc);
}

ssize_t Connection::Check(){
    return server->Check(acc);
}

bool Connection::FdSet(int fd){
    return server->FdSet(acc, fd);
}

std::vector<std::string> Connection::LookList(){
    return server->LookList(acc);
}

bool Connection::Find(const char* str){
    return server->Find(str);
}
void Connection::AddShip(const char* str){
    server->AddShip(acc, str);
}