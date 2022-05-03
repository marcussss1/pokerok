#pragma once

#include <algorithm>
#include <arpa/inet.h>
#include <boost/program_options.hpp>
#include <cstring>
#include <cstddef>
#include <sys/epoll.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <string>
#include <set>
#include <thread>

class Server 
{
public:
    Server();
    ~Server();
    void run();
// private:
    void CreateSocket();
    void Bind();
    void Listen();
    // void CreateEvent();
    void StartGame();
    bool SocketIsConnected(int socket);
    int SetNonblock(int fileDescriptor);

    int masterSocket_ = 0;
    struct sockaddr_in addr_ = {};
    int ePoll_ = 0;
    struct epoll_event events_ = {};
    std::set<int> clientSockets_ = {};
};