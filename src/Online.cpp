#include "Online.h"
#include "Connection.h"
#include <vector>
#include <string>
#include <iostream>

Online::Online(Connection* _conn) : conn(_conn), state(Online_State::Home){

    std::function<void()> cb = std::bind(&Online::Onl, this);
    conn->SetOnLineCallback(cb);
    
}

Online::~Online(){}

void Online::Onl(){
    conn->Read();
    if(state == Online_State::Home){
        if(*(conn->Readp()) == '1'){
            conn->sBuf_append("\n", 1);
            std::vector<std::string> v = conn->LookList();
            for(int i = 0; i < v.size(); ++ i){
                conn->sBuf_append(v[i].c_str(), v[i].size());
                conn->sBuf_append("\n", 1);
            }
            conn->append(" 1 Look friend list\n 2 Send message\n 3 Add friend\n");
            conn->Send();
        }
        else if(*(conn->Readp()) == '2'){
            conn->Send_str("\nplease input his(her) name:\n");
            state = Online_State::Add;
        }
        else if(*(conn->Readp()) == '3'){

        }
    }
    else if(state == Online_State::Add){
        if(conn->Find(conn->Readp())){
            conn->Send_str("\nAdd successfully\n 1 Look friend list\n 2 Send message\n 3 Add friend\n");
            conn->AddShip(conn->Readp());
        }
        else
            conn->Send_str("\nUser don't exist\n 1 Look friend list\n 2 Send message\n 3 Add friend\n");
            
        state == Online_State::Home;
    }
}