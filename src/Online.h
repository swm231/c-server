#ifndef _ONLINE_H_
#define _ONLINE_H_

enum Online_State{
    Home,
    Add,
    Sending,
};

class Connection;
class Online{
private:
    Connection* conn;
    Online_State state;
public:
    Online(Connection*);
    ~Online();

    void Onl();
};

#endif