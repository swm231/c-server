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
    conn->sBuf_append(" 1 Look online list\n 2 Send message\n 3 Quit system\n");
    conn->Send(sock->GetFd());
    state = Online_State::Home;
}

void Online::Send(int Fd){
    conn->sBuf_append("[");
    conn->sBuf_append(acc->name_.c_str());
    conn->sBuf_append("]:\n");
    conn->Send(Fd);
    conn->SendR(Fd);
    conn->sBuf_append("\n");
    conn->Send(Fd);
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
            conn->Send_str("\n 1 Private chat\n 2 Public chat:\n");
            state = Online_State::wSend;
        }
        else if(*(conn->Readp()) == '3'){
            // quit system
        }
    }
    else if(state == Online_State::wSend){
        if(*(conn->Readp()) == '1'){
            state = Online_State::priSend;
            conn->Send_str("\nWho will you send:\n");
        }
        else if(*(conn->Readp()) == '2') {
            conn->Send_str("\nMessage:\n");
            state = Online_State::Sending;
        }
    }
    else if(state == Online_State::priSend){
        fd = conn->FindFd(conn->Readp());
        if(fd == -1){
            conn->Send_str("\nUesr don't online\n");
            Home();
        }
        else if(fd > 0){
            str.append(conn->Readp());  //要发给谁
            conn->Send_str("\nMessage:\n");
            state = Sending;
        }
        else Home();
    }
    else if(state == Online_State::Sending){
        // 发送消息
        if(str.size()) fd = conn->FindFd(str.c_str());
        else fd = 0;
        if(fd == -1){
            conn->sBuf_append("\nSend unsuccessfully\nUser outline\n");
            Home();
        }
        else{
            if(fd > 0)
                Send(fd);
            else if(fd == 0){
                std::vector<int> v = conn->GetOnlFd();
                for(int i = 0; i < v.size(); ++ i){
                    if(v[i] == sock->GetFd()) continue;
                    Send(v[i]);
                }
            }
            conn->Send_str("\nSend successfully!!!\n 1 Home\n 2 Continue\n");
            state = Online_State::Sended;
        }
    }
    else if(state == Online_State::Sended){
        if(*(conn->Readp()) == '1'){
            Home();
            str.clear();
        }
        else {
            conn->Send_str("\nMessage:\n");
            state = Online_State::Sending;
        }
    }
}
