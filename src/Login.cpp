#include "Login.h"
#include "Connection.h"
#include "Channel.h"
#include <iostream>

LogIn::LogIn(Connection* _conn, Channel* _ch) : conn(_conn), ch(_ch), 
    lstate(LogState::lInvalid), istate(SignIn::iInvalid), ostate(SignOn::oInvalid){

    std::function<void()> cb = std::bind(&LogIn::Log, this);
    ch->SetReadCallback(cb);
}

LogIn::~LogIn(){}

void LogIn::Log(){
    conn->Read();
    if(lstate == LogState::lInvalid){
        if(*(conn->Readp()) == '1') {//登录
            lstate = LogState::lSignIn;
            conn->Send_str("Please input user:\n");
        }
        else if(*(conn->Readp()) == '2') {//注册
            lstate = LogState::lSignon;
            conn->Send_str("Please input user:\n");
        }
    }
    else if(lstate == LogState::lSignIn){
        if(istate == SignIn::iInvalid){
            conn->Set_Name();
            if(conn->Check() == 1){
                conn->Send_str("Please input passward:\n");
                istate = SignIn::iUser;
            }
            else if(conn->Check() == 2){
                conn->Send_str("User don't exist\nPlease input user again:\n");
            }
        }
        else if(istate == SignIn::iUser){
            conn->Set_Name();
            if(conn->Check() == 0){
                // 登录成功！！；
                istate = SignIn::iPass;
            }
            else if(conn->Check() == 1){
                conn->Send_str("Wrong passward\nPlease input passward again:\n");
            }
        }
    }
    else if(lstate == LogState::lSignon){
        if(ostate == SignOn::oInvalid){
            conn->Set_Name();
            if(conn->Check() == 2){
                conn->Send_str("Please input passward firstly:\n");
                conn->Insert();
                ostate = SignOn::oUser;
            }
            else if(conn->Check() == 1){
                conn->Send_str("User has existed\nPlease input user again:\n");
            }
        }
        else if(ostate == SignOn::oUser){
            conn->Set_Pass();
            conn->Modify();
            conn->Send_str("Please input passward again:\n");
            ostate = SignOn::oPass;
        }
        else if(ostate == SignOn::oPass){
            conn->Set_Pass();
            if(conn->Check() == 0){
                //成功！！；
                ostate = SignOn::rPass;
            }
            else if(conn->Check() == 1){
                conn->Send_str("Wrong passward\nPlease input passward firstly:\n");
                ostate = SignOn::oUser;
            }
        }
    }
}