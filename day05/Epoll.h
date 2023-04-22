#pragma once
#include "Channel.h"
#include <sys/epoll.h>
#include <vector>

class Epoll
{
private:
    int epfd;
    struct epoll_event *events;
public:
    Epoll();
    ~Epoll();

    void addFd(int fd, uint32_t op);
    void updateChannel(Channel*);
    std::vector<Channel*> poll(int timeout = -1);
};
