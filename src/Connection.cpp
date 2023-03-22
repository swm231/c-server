#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "util.h"
#include "Mysql.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Server.h"
#include <unistd.h>
#include <string.h>

#define MAX_BUFFER 1024

Connection::Connection(Server* _server, EventLoop* _loop, Socket* _sock) 
        : server(_server), loop(_loop), sock(_sock), ch(new Channel(loop, sock->GetFd())),
          acc(new Account()),ReadBuffer(new Buffer()), SendBuffer(new Buffer()), 
          state(State::Invalid), lstate(LogState::lInvalid), istate(SignIn::iInvalid), ostate(SignOn::oInvalid){

    state = State::Connected;

    ch->enablereading();
    ch->useET();
    std::function<void()> cb = std::bind(&Connection::LogIn, this);
    ch->SetReadCallback(cb);
    Send_str("Successfully connecting to the server\n 1 Sign in\n 2 Sign on\n");
}

void Connection::LogIn(){
    Read();
    if(lstate == LogState::lInvalid){
        if(*(ReadBuffer->c_str()) == '1') {//登录
            lstate = LogState::lSignIn;
            Send_str("Please input user:\n");
        }
        else if(*(ReadBuffer->c_str()) == '2') {//注册
            lstate = LogState::lSignon;
            Send_str("Please input user:\n");
        }
    }
    else if(lstate == LogState::lSignIn){
        if(istate == SignIn::iInvalid){
            acc->Set_Name(ReadBuffer->c_str());
            if(Check() == 1){
                Send_str("Please input passward:\n");
                istate = SignIn::iUser;
            }
            else if(Check() == 2){
                Send_str("User don't exist\nPlease input user again:\n");
            }
        }
        else if(istate == SignIn::iUser){
            acc->Set_Pass(ReadBuffer->c_str());
            if(Check() == 0){
                // 登录成功！！；
                istate = SignIn::iPass;
            }
            else if(Check() == 1){
                Send_str("Wrong passward\nPlease input passward again:\n");
            }
        }
    }
    else if(lstate == LogState::lSignon){
        if(ostate == SignOn::oInvalid){
            acc->Set_Name(ReadBuffer->c_str());
            if(Check() == 2){
                Send_str("Please input passward firstly:\n");
                ostate = SignOn::oUser;
            }
            else if(Check() == 1){
                Send_str("User has existed\nPlease input user again:\n");
            }
        }
        else if(ostate == SignOn::oUser){
            acc->Set_Pass(ReadBuffer->c_str());
            Insert();
            Send_str("Please input passward again:\n");
            ostate = SignOn::oPass;
        }
        else if(ostate == SignOn::oPass){
            acc->Set_Pass(ReadBuffer->c_str());
            printf("%d\n", Check());
            if(Check() == 0){
                //成功！！；
                ostate = SignOn::rPass;
            }
            else if(Check() == 1){
                Send_str("Wrong passward\nPlease input passward firstly:\n");
                ostate = SignOn::oUser;
            }
        }
    }
}

Connection::~Connection(){
    delete ch;
    delete sock;
    delete ReadBuffer;
    delete SendBuffer;
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
        if(read_bytes > 0)
            ReadBuffer->append(buf, read_bytes);
        else if(read_bytes == -1 && errno == EINTR)
            continue;
        else if(read_bytes == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
            break;
        else if(read_bytes == 0){
            printf("EOF, client fd %d disconnection\n", sock->GetFd());
            //do task
            close(sock->GetFd());
            break;
        }
    }
}

void Connection::SetDeleteConnectionCallback(std::function<void(Socket*)> cb){
    DeleteConnectionCallback = cb;
}

// void Connection::LogIn(){
//     // 连接成功后任务:确认登录信息
//     Send_str("Successfully connecting to the server\n 1 Sign in\n 2 Sign on\n");

//     if(*(ReadBuffer->c_str()) == '1'){
//         //登录

//     }
//     else if(*(ReadBuffer->c_str()) == '2'){
//         //注册
//         Send_str("please input user:\n");

//         bool legal_name = false;
//         while(!legal_name){  
//             acc->Set_Name(ReadBuffer->c_str());
//             if(Check() == 1){
//                 Send_str("The user name already exists\nplease input user:\n");
//             }
//             else if(Check() == 2)
//                 legal_name = true;
//         }
//         Insert();

//         bool legal_pass = false;
//         bool frt = true;
//         while(!legal_pass){
//             if(frt) Send_str("please input passward fristly:\n");
//             frt = false;
//             acc->Set_Pass(ReadBuffer->c_str());

//             Modify();

//             Send_str("please input passward again:\n");   
//             acc->Set_Pass(ReadBuffer->c_str());

//             if(Check() == 0)  //成功！！
//                 legal_pass = true;
//             else if(Check() == 1)
//                 Send_str("passward worng\nplease input passward fristly:\n");
//         }
//     }
    
//     //OnConnectionCallback(this);
//     // 登录完之后任务
// }

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