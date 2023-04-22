#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include <functional>
#include <iostream>

Acceptor::Acceptor(EventLoop *_loop) :  loop(_loop), sock(nullptr), acceptChannel(nullptr)
{
    //建立Acceptor的socket
    sock = new Socket();
    InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    sock->bind(addr);
    sock->listen();
    sock->setnonblocking();
    //把处理连接的channel与loop绑定
    acceptChannel = new Channel(loop, sock->getFd());
    //设置当epoll上的acceptChannel有事件时，应该调用cb
    std::function<void()> cb = std::bind(&Acceptor::acceptConnection, this);
    acceptChannel->setCallback(cb);
    //开始监听
    acceptChannel->enableReading();
    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock;
    delete acceptChannel;
}

//acceptChannel有事件时应该调用的函数，即建立客户端连接的函数
void Acceptor::acceptConnection()
{
    InetAddress *clnt_addr = new InetAddress();
    Socket *clnt_sock = new Socket(sock->accept(clnt_addr));
    printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->getAddr().sin_addr), ntohs(clnt_addr->getAddr().sin_port));
    clnt_sock->setnonblocking();
    //建立连接后，调用相应的回调函数，上面是Acceptor负责的，下面的回调函数是Server负责的，因此这里要回调，让Server去调用
    newConnectionCallback(clnt_sock);
    delete clnt_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket*)> _cb)
{
    newConnectionCallback = _cb;
}