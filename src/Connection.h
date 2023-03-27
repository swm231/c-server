#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>
#include <vector>
#include <string>

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
    Socket* sock;
    Channel* ch;
    Account* acc;
    State state;
    LogIn* login;
    Online* onl;
    std::function<void(Socket*)> DeleteConnectionCallback;
    std::function<void()> OnLineCallback;
public:
    Buffer* ReadBuffer;
    Buffer* SendBuffer;
    Connection(Server*, EventLoop*, Socket*);
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
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
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