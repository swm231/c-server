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
    Buffer* ReadBuffer;
    Buffer* SendBuffer;
    State state;
    LogIn* login;
    Online* onl;
    std::function<void(Socket*)> DeleteConnectionCallback;
    
public:
    std::function<void()> OnLineCallback;
    Connection(Server*, EventLoop*, Socket*);
    ~Connection();

    void Send_str(const char*);
    void Send();
    void Read();
    void sBuf_append(const char*, int);
    
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
    void SetOnLineCallback(std::function<void()>);
    void Set_Online_channel();
    void Close();

    const char* Readp();
    void Set_Name();
    void Set_Pass();
    void Set_State(State);

    State GetState();

    bool Insert();
    bool Delete();
    bool Modify();
    ssize_t Check();  //0:全部正确  1:用户名正确  2:全不正确
    bool FdSet(int);

    std::vector<std::string> LookList();
    bool Find(const char*);
    void AddShip(const char*);
};

#endif