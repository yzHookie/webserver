#pragma once
#include "Epoll.h"
#include <sys/epoll.h>
#include <functional>

class EventLoop;
//虽然把这些成员写到Socket类也能完成功能，但是Socket类主要负责创建、绑定、监听socket，Channel类主要负责IO事件的关注
//分成两个类，耦合性较低
class Channel
{
private:
    EventLoop *loop;
    int fd;
    uint32_t events;
    uint32_t revents;
    bool inEpoll;
    std::function<void()> callback;
public:
    Channel(EventLoop *_loop, int _fd);
    ~Channel();
    
    void handleEvent();
    void enableReading();

    int getFd();
    uint32_t getEvents();
    uint32_t getRevents();
    bool getInEpoll();
    void setInEpoll();
    
    void setRevents(uint32_t);
    void setCallback(std::function<void()>);
};