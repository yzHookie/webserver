#pragma once
#include "Socket.h"
#include <arpa/inet.h>
#include <netinet/in.h>

class InetAddress
{
public:
    struct sockaddr_in addr;
    socklen_t addr_len;
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();
};