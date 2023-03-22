#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <functional>

class EventLoop;
class Socket;
class Buffer;
class Channel;
class Server;
class Account;
enum State{
    Invalid,
    Handshaking,
    Connected,
    DisConnected,
    Closed,
    Failed,
};
enum LogState{
    lInvalid,
    lSignIn,
    lSignon,
};
enum SignIn{
    iInvalid,
    iUser,
    iPass,
};
enum SignOn{
    oInvalid,
    oUser,
    oPass,
    rPass,
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
    LogState lstate;
    SignIn istate;
    SignOn ostate;
    std::function<void(Connection*)> OnConnection;
    std::function<void(Socket*)> DeleteConnectionCallback;
public:
    Connection(Server*, EventLoop*, Socket*);
    ~Connection();

    void Send_str(const char*);
    void Send();
    void Read();

    void LogIn();
    
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
    void SetOnConnectionCallback(std::function<void(Connection*)>);
    void Close();

    State GetState();

    bool Insert();
    bool Delete();
    bool Modify();
    ssize_t Check();  //0:全部正确  1:用户名正确  2:全不正确
};

#endif