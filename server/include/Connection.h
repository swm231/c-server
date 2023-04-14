#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "ThreadCache.h" 
#include <functional>
#include <vector>
#include <string>
#include <memory>

class EventLoop;
class Socket;
class Buffer;
class Channel;
class Online;
class Server;
class LogIn;
class Account;
enum State{
    Invalid,
    Connected,
    DisConnected,
    Closed,
};

class Connection{
private:
    Server* server;
    EventLoop* loop;
    std::shared_ptr<Socket> sock;
    State state;

    Channel* ch;
    Account* acc;
    std::unique_ptr<LogIn> login;
    std::unique_ptr<Online> onl;
    std::unique_ptr<Buffer> ReadBuffer;
    std::unique_ptr<Buffer> SendBuffer;
    std::function<void(std::shared_ptr<Socket>)> DeleteConnectionCallback;
    std::function<void()> OnLineCallback;
public:
    Connection(Server*, EventLoop*, std::shared_ptr<Socket>);
    ~Connection();
    //Buffer
    void Send_str(const char*);
    void Send(int);
    void SendFd(const char*, int);
    void SendR(int);
    void Read();
    void sBuf_append(const char*);
    const char* Readp();
    //Connection
    void SetDeleteConnectionCallback(std::function<void(std::shared_ptr<Socket>)>);
    void SetOnLineCallback(std::function<void()>);
    void Set_Online_channel();
    void Close();
    //Accoount
    void Set_Name();
    void Set_Pass();
    void Set_State(State);

    State GetState();
    //Mysql
    bool Insert();
    bool Delete();
    bool Modify();
    ssize_t Check();  //0:全部正确  1:用户名正确  2:全不正确
    bool FdSet(int);

    std::vector<std::string> LookList();
    std::vector<int> GetOnlFd();
    bool Find(const char*);
    void AddShip(const char*);
    int FindFd(const char*);
};

#endif