#include "Login.h"
#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include <iostream>

LogIn::LogIn(Connection* _conn, Channel* _ch, Socket* _sock) : conn(_conn), ch(_ch), sock(_sock),
    lstate(LogState::lInvalid), istate(SignIn::iInvalid), ostate(SignOn::oInvalid){
    
    ch->enablereading();
    ch->useET();
    std::function<void()> cb = std::bind(&LogIn::Log, this);
    ch->SetReadCallback(cb);
}

LogIn::~LogIn(){}

void LogIn::Log(){
    conn->Read();
    if(lstate == LogState::lInvalid){
        if(*(conn->Readp()) == '1') {//登录
            lstate = LogState::lSignIn;
            conn->Send_str("\nPlease input user:\n");
        }
        else if(*(conn->Readp()) == '2') {//注册
            lstate = LogState::lSignon;
            conn->Send_str("\nPlease input user:\n");
        }
    }
    else if(lstate == LogState::lSignIn){
        if(istate == SignIn::iInvalid){
            conn->Set_Name();
            if(conn->Check() == 1){
                conn->Send_str("\nPlease input passward:\n");
                istate = SignIn::iUser;
            }
            else if(conn->Check() == 2){
                conn->Send_str("\nUser don't exist\nPlease input user again:\n");
            }
        }
        else if(istate == SignIn::iUser){
            conn->Set_Pass();
            if(conn->Check() == 0){
                LogSuccess();
                istate = SignIn::iPass;
            }
            else if(conn->Check() == 1){
                conn->Send_str("\nWrong passward\nPlease input passward again:\n");
            }
        }
    }
    else if(lstate == LogState::lSignon){
        if(ostate == SignOn::oInvalid){
            conn->Set_Name();
            if(conn->Check() == 2){
                conn->Send_str("\nPlease input passward firstly:\n");
                conn->Insert();
                ostate = SignOn::oUser;
            }
            else if(conn->Check() == 1){
                conn->Send_str("\nUser has existed\nPlease input user again:\n");
            }
        }
        else if(ostate == SignOn::oUser){
            conn->Set_Pass();
            conn->Modify();
            conn->Send_str("\nPlease input passward again:\n");
            ostate = SignOn::oPass;
        }
        else if(ostate == SignOn::oPass){
            conn->Set_Pass();
            if(conn->Check() == 0){
                LogSuccess();
                ostate = SignOn::rPass;
            }
            else if(conn->Check() == 1){
                conn->Send_str("\nWrong passward\nPlease input passward firstly:\n");
                ostate = SignOn::oUser;
            }
        }
    }
}

void LogIn::FdSet(){
    conn->FdSet(sock->GetFd());
}

void LogIn::LogSuccess(){
    conn->Send_str("\nWelcome to OO:\n 1 Look friend list\n 2 Send message\n 3 Add friend\n");
    FdSet();
    conn->Set_State(State::Connected);
    conn->Set_Online_channel();
}