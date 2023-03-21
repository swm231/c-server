#include "EventLoop.h"
#include "Server.h"
#include "Mysql.h"
#include "Channel.h"
#include "ThreadPool.h"
#include "Connection.h"
#include "Socket.h"
#include <string>
#include <iostream>
//g++ *.cpp -o main -lpthread `mysql_config --cflags --libs`
int main(){
    EventLoop* loop = new EventLoop();
    Server* server = new Server(loop);

    server->OnConnect([](Connection* conn){
        while(true){
            conn->Read();
            if(conn->GetState() == State::DisConnected){
                conn->Close();
                break;
            }
        }
    });

    loop->loop();

    delete loop;
    delete server;
    return 0;
}