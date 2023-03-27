#ifndef _ONLINE_H_
#define _ONLINE_H_

#include <string>

enum Online_State{
    Home,
    priSend,
    wSend,
    Sending,
    Sended,
};

class Socket;
class Account;
class Connection;
class Online{
private:
    std::string str;
    int fd;
    Account* acc;
    Socket* sock;
    Connection* conn;
    Online_State state;
public:
    Online(Connection*, Socket*, Account*);
    ~Online();

    void Onl();
    void Home();
    void Send(int);
};

#endif