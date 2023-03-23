#include "Online.h"
#include "Socket.h"
#include "Buffer.h"
#include "Connection.h"
#include "Mysql.h"
#include <vector>
#include <string>
#include <iostream>

Online::Online(Connection* _conn, Socket* _sock, Account* _acc) 
    : conn(_conn), state(Online_State::Home), sock(_sock), acc(_acc){

    std::function<void()> cb = std::bind(&Online::Onl, this);
    conn->SetOnLineCallback(cb);
    
}

Online::~Online(){}

void Online::Home(){
    conn->sBuf_append(" 1 Look friend list\n 2 Send message\n 3 Add friend\n");
    conn->Send(sock->GetFd());
    state = Online_State::Home;
}

void Online::Onl(){
    conn->Read();
    if(state == Online_State::Home){
        if(*(conn->Readp()) == '1'){
            conn->sBuf_append("\n");
            std::vector<std::string> v = conn->LookList();
            for(int i = 0; i < v.size(); ++ i){
                conn->sBuf_append(v[i].c_str());
                conn->sBuf_append("\n");
            }
            Home();
        }
        else if(*(conn->Readp()) == '2'){
            conn->Send_str("\nWho are you send to:\n");
            state = Online_State::wSend;
        }
        else if(*(conn->Readp()) == '3'){
            conn->Send_str("\nplease input his(her) name:\n");
            state = Online_State::Add;
        }
    }
    else if(state == Online_State::Add){
        if(conn->Find(conn->Readp())){
            conn->sBuf_append("\nAdd successfully\n");
            conn->AddShip(conn->Readp());
        }
        else
            conn->sBuf_append("\nUser don't exist\n");
        Home();
    }
    else if(state == Online_State::wSend){
        fd = conn->FindFd(conn->Readp());

        int _isf; // 是不是好友
        if(!_isf) {
            conn->sBuf_append("\nThis user isn't your friend\n");
            Home();
        }
        else if(fd == -1){
            conn->Send_str("\nFriend don't online\n");
            Home();
        }
        else if(fd > 0 && _isf){
            str.append(conn->Readp());  //要发给谁
            conn->Send_str("\nMessage:\n");
            state = Sending;
        }
        else Home();
    }
    else if(state == Online_State::Sending){
        // 发送消息
        fd = conn->FindFd(str.c_str());
        if(fd == -1){
            conn->sBuf_append("\nSend unsuccessfully\nUser outline\n");
            Home();
        }
        else{
           // conn->Send(false, fd);
            conn->sBuf_append("[");
            conn->sBuf_append(acc->name_.c_str());
            conn->sBuf_append("]:\n");
            conn->Send(fd);
            conn->SendR(fd);
            conn->Send_str("\nSend successfully!!!\n 1 Home\n 2 Continue\n");
            state = Sended;
        }
    }
    else if(state == Online_State::Sended){
        if(*(conn->Readp()) == '1')
            Home();
        else {
            conn->Send_str("\nMessage:\n");
            state = Sending;
        }
    }

}