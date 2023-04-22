#include "Connection.h"
#include "Socket.h"
#include "Channel.h"
#include <functional>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock), channel(nullptr)
{
    //把该channel 与 loop 绑定
    channel = new Channel(loop, sock->getFd());
    //设置该Connection（channel）有事件时，应该做的事情（回声）
    std::function<void()> cb = std::bind(&Connection::echo, this, sock->getFd());
    channel->setCallback(cb);
    //把该channel放到loop的红黑树中开始监听
    channel->enableReading();
}

Connection::~Connection()
{
    delete channel;
    delete sock;
}

void Connection::echo(int sockfd)
{
    char buf[READ_BUFFER];
    while(true)
    {
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0)
        {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        }
        else if (bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK)))
        {
            printf("finish reading once, errno: %d\n", errno);
            break;
        }
        else if (bytes_read == 0)
        {
            printf("EOF, client fd %d disconnected\n", sockfd);
            // close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
            //回调函数，当连接中断时让Server调用删除连接的函数
            deleteConnectionCallback(sock);
            break;
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(Socket*)> _cb)
{
    deleteConnectionCallback = _cb;
}