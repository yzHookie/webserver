#include "Server.h"
#include "Channel.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include <functional>

Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr)
{
    //创建acceptor来处理连接请求
    acceptor = new Acceptor(loop);
    //当acceptor有新连接时，在调用accpetChannel的回调函数建立连接后（Acceptor要做的），Server还应该做的事情（把Connection与loop绑定，绑定删除Connection的函数）
    std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
    acceptor->setNewConnectionCallback(cb);
}

Server::~Server()
{
    delete acceptor;
}
void Server::newConnection(Socket *sock)
{
    //把客户端socket与当前Server的loop绑定，存入连接池中
    Connection *conn = new Connection(loop, sock);
    //把删除连接的函数绑定到Connection中
    std::function<void(Socket*)> cb = std::bind(&Server::deleteConnection, this, std::placeholders::_1);
    conn->setDeleteConnectionCallback(cb);
    connections[sock->getFd()] = conn;
}

void Server::deleteConnection(Socket *sock)
{
    Connection *conn = connections[sock->getFd()];
    connections.erase(sock->getFd());
}