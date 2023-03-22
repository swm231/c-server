#ifndef _LOGIN_H_
#define _LOGIN_H_


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
class Connection;
class Channel;
class LogIn{
private:
    Channel* ch;
    Connection* conn;    
    LogState lstate;
    SignIn istate;
    SignOn ostate;
public:
    LogIn(Connection*, Channel*);
    ~LogIn();

    void Log();
};

#endif