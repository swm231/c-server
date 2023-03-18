#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <sys/epoll.h>

class Epoll;
class Channel{
private:
    Epoll* ep;
    int fd;
    uint32_t event;
    uint32_t revent;
    bool inEpoll;
public:
    Channel(Epoll*, int);
    ~Channel();

    void enablereading();

    int GetFd();
    uint32_t GetEvent();
    uint32_t GetRevent();
    bool GetStatus();
    void SetStatus();
    void SetRevent(uint32_t);

};

#endif