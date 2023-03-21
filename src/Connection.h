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
    Invalid = 1,
    Handshaking,
    Connected,
    DisConnected,
    Closed,
    Failed,
};

class Connection{
private:
    Server* server;
    EventLoop* loop;
    Socket* sock;
    Channel* ch;
    Buffer* ReadBuffer;
    Buffer* SendBuffer;
    State state{State::Invalid};
    std::function<void(Connection*)> OnConnectionCallback;
    std::function<void(Socket*)> DeleteConnectionCallback;
public:
    Connection(Server*, EventLoop*, Socket*);
    ~Connection();

    void Send(int);
    void Read();
    void SetDeleteConnectionCallback(std::function<void(Socket*)>);
    void SetOnConnectionCallback(std::function<void(Connection*)>);
    void Close();

    State GetState();

    bool Insert(Account&&);
    bool Delete(Account&&);
    bool Modify(Account&&);
    bool Check(Account&&);
};

#endif