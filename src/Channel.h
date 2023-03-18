#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <sys/epoll.h>
#include <functional>

class EventLoop;
class Channel{
private:
    EventLoop* loop;
    int fd;
    uint32_t event;
    uint32_t revent;
    bool inEpoll;
    std::function<void()> callback;
public:
    Channel(EventLoop*, int);
    ~Channel();

    void enablereading();

    int GetFd();
    uint32_t GetEvent();
    uint32_t GetRevent();
    bool GetStatus();
    void SetStatus();
    void SetRevent(uint32_t);
    void SetCallback(std::function<void()>);
    void HandleEvent();
};

#endif