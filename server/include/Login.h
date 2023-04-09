#ifndef _LOGIN_H_
#define _LOGIN_H_

#include <memory>

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
class Socket;
class Connection;
class Channel;
class LogIn{
private:
    std::shared_ptr<Socket> sock;
    Channel* ch;
    Connection* conn;
    LogState lstate;
    SignIn istate;
    SignOn ostate;
public:
    LogIn(Connection*, Channel*, std::shared_ptr<Socket>);
    ~LogIn();

    void Log();
    void FdSet();
    void LogSuccess();
};

#endif