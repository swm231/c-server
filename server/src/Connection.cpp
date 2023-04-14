#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Mysql.h"
#include "Online.h"
#include "Channel.h"
#include "Login.h"
#include "EventLoop.h"
#include "ThreadCache.h" 
#include "Server.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(Server* _server, EventLoop* _loop, std::shared_ptr<Socket> _sock) 
        : server(_server), loop(_loop), sock(_sock),
          state(State::Invalid) {
        
    ch = new (ThreadCache::operator new (sizeof(Channel))) Channel(loop, sock->GetFd());
    acc = new (ThreadCache::operator new (sizeof(Account))) Account();
    ReadBuffer = std::make_unique<Buffer>();
    SendBuffer = std::make_unique<Buffer>();
    login = std::make_unique<LogIn>(this, ch, sock);
    onl = std::make_unique<Online>(this, sock, acc);

    Send_str("Successfully connecting to the server\n 1 Sign in\n 2 Sign on\n");
}

Connection::~Connection(){
    ThreadCache::operator delete (ch, sizeof(*ch));
    ThreadCache::operator delete (acc, sizeof(*acc));
}

void Connection::Send_str(const char* str){
    SendBuffer->SetBuf(str);
    Send(sock->GetFd());
    SendBuffer->clear();
}

void Connection::Send(int fd){
    //int fd = sock->GetFd();
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

void Connection::SendFd(const char* str, int fd){
    SendBuffer->SetBuf(str);
    Send(fd);
}

void Connection::SendR(int fd){
    SendBuffer->SetBuf(ReadBuffer->c_str());
    Send(fd);
}

void Connection::sBuf_append(const char* str){
    SendBuffer->append(str);
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

void Connection::SetDeleteConnectionCallback(std::function<void(std::shared_ptr<Socket>)> cb){
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

std::vector<int> Connection::GetOnlFd(){
    return server->GetOnlFd();
}

bool Connection::Find(const char* str){
    return server->Find(str);
}
void Connection::AddShip(const char* str){
    server->AddShip(acc, str);
}

int Connection::FindFd(const char* str){
    return server->FindFd(str);
}