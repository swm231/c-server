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
    uint32_t ready;
    bool inEpoll;
    //bool useThreadPool;
    std::function<void()> ReadCallback;
    std::function<void()> WriteCallback;
public:
    Channel(EventLoop*, int);
    ~Channel();

    void HandleEvent();
    void enablereading();

    int GetFd();
    uint32_t GetEvent();
    uint32_t GetReady();
    bool GetInEpoll();
    void SetInEpoll(bool _in = true);
    void useET();

    void SetReady(uint32_t);
    void SetReadCallback(std::function<void()>);
    // void SetUseThreadPool(bool);
};

#endif