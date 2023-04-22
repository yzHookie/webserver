#include <asm-generic/errno-base.h>
#include <asm-generic/errno.h>
#include <stdio.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

void handleReadEvent(int);

int main() {
    Socket *serv_sock = new Socket();
    InetAddress *serv_addr = new InetAddress("127.0.0.1", 8888);
    serv_sock->bind(serv_addr);
    serv_sock->listen();
    serv_sock->setNonBlocking();

    Epoll *ep = new Epoll();
    //设置边缘触发
    ep->addFd(serv_sock->getFd(), EPOLLIN | EPOLLET);
    while(true) {
        //epoll当前触发的I/O事件
        std::vector<epoll_event> events = ep->poll();
        int cnt = events.size();
        for (int i = 0; i < cnt; i++) {
            //有新连接
            if (events[i].data.fd == serv_sock->getFd()) {
                InetAddress *clnt_addr = new InetAddress(); //暂时没有delete，会泄露
                Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr)); //暂无delete
                printf("new client fd %d! IP : %s Port : %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
                clnt_sock->setNonBlocking();
                ep->addFd(clnt_sock->getFd(), EPOLLIN | EPOLLET);
            } else if (events[i].events && EPOLLIN) { //可读事件
                handleReadEvent(events[i].data.fd);
            } else { //后续会补充的其他事件
                printf("something else happened\n");
            }
        }
    }
    delete serv_sock;
    delete serv_addr;
    return 0;
}

void handleReadEvent(int sockfd) {
    char buf[READ_BUFFER];
    while(true) { //非阻塞是I/O,读取客户端buffer，一次读取buf大小，直到全部读完
        bzero(&buf, sizeof(buf));
        ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
        if (bytes_read > 0) {
            printf("message from client fd %d: %s\n", sockfd, buf);
            write(sockfd, buf, sizeof(buf));
        } else if (bytes_read == -1 && errno == EINTR) { //客户端正常中断，继续读取
            printf("continue reading");
            continue;
        } else if (bytes_read == -1 && ((errno == EAGAIN) || (errno== EWOULDBLOCK))) {
            //在非阻塞式I/O中表示已经读取完
            printf("finish reading once, errno: %d\n", errno);
            break;
        } else if (bytes_read == 0) { //EOF，客户端断开连接
            printf("EOF, client fd %d diconnected\n", sockfd);
            close(sockfd);
            break;
        }
    }
}