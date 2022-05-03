// #include <arpa/inet.h>
// #include <boost/program_options.hpp>
// #include <cstring>
// #include <cstddef>
// #include <sys/epoll.h>
// #include <fcntl.h>
// #include <iostream>
// #include <unistd.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <sys/types.h>
// #include <sys/un.h>
// #include <stdio.h>
// #include <set>
// #include <thread>

// #define MAX_EVENTS 1024

// namespace po = boost::program_options;
// namespace 
// {
//     const unsigned kNumberOfThreads = 4;
// } // namespace

// int setNonblock(int fileDescriptor)
// {
//     int flags;
// #if defined(O_NONBLOCK)
//     if (-1 == (flags = fcntl(fileDescriptor, F_GETFL, 0)))
//         flags = 0;
//     return fcntl(fileDescriptor, F_SETFL, O_NONBLOCK);
// #else
//     flags = 1;
//     return ioctl(fileDescriptor, FIONBIO, &flags);
// #endif
// }

// bool SocketIsConnected(int socket)
// {
//     int Errors_sock = send(socket, "s ", 0, 1);
//     Errors_sock = send(socket, "s ", 0, 1);
//     if (Errors_sock == -1)
//     {
//         return false;
//         close(socket);
//     }
//     return true;
// }

// void HandleClient()
// {
//     std::cout << "\nАсинхронная функция обработки клиента...\n";
// }
#include "server.hpp"

int main() 
{
    Server a;
    a.run();
    return 0;
}
    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(12345);
    // const char* adddr = "127.0.0.1";
    // addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // std::cout << "Попытка подключения...\n";
    // int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // if (MasterSocket == -1) {
    //     std::cerr << "Ошибка при создании сокета\n";
    //     return -1;
    // }
    // if(bind(MasterSocket, (struct sockaddr*)(&addr), sizeof(addr)) == -1) {
    //     close(MasterSocket);
    //     std::cerr << "Ошибка при подключении к клиенту\n";
    //     return -1;
    // }
    // setNonblock(MasterSocket);
    // if (listen(MasterSocket, SOMAXCONN) == -1) {
    //     close(MasterSocket);
    //     std::cerr << "Ошибка при прослушивании сокета\n";
    //     return -1;
    // }
    // int EPoll = epoll_create1(0);
    // struct  epoll_event Event;
    // Event.data.fd = MasterSocket;
    // Event.events = EPOLLIN;
    // epoll_ctl(EPoll, EPOLL_CTL_ADD, MasterSocket, &Event);
    // std::set<int> sockets;
    // while(1)
    // {
    //     for (auto it : sockets)
    //         if (!SocketIsConnected(it))
    //         {
    //             std::cout << "\n\nКлиент отключился...  " << "Slave socket: " << it << "\n\n";
    //             sockets.erase(it);
    //             break;
    //         }
    //     struct epoll_event Events[MAX_EVENTS];
    //     int N = epoll_wait(EPoll, Events, MAX_EVENTS, -1);
    //     for (size_t i = 0; i < N; ++i)
    //     {
    //         if (Events[i].data.fd == MasterSocket)
    //         {
    //             int SlaveSocket = accept(MasterSocket, 0, 0);
    //             sockets.insert(SlaveSocket);
    //             std::cout << "\n\nКлиент подключился...  " << "Slave socket: " << SlaveSocket << "\n\n";
    //             setNonblock(SlaveSocket);
    //             struct epoll_event Event;
    //             Event.data.fd = SlaveSocket;
    //             Event.events = EPOLLIN;
    //             epoll_ctl(EPoll, EPOLL_CTL_ADD, SlaveSocket, &Event);
    //         }
    //         else
    //         {
    //             char buf[1024];
    //             int res = recv(Events[i].data.fd, buf, sizeof(buf), MSG_NOSIGNAL);
    //             if ((res == 0) && (errno != EAGAIN))
    //             {
    //                 shutdown(Events[i].data.fd, SHUT_RDWR);
    //                 close(Events[i].data.fd);
    //             }
    //             else if (res > 0) 
    //             { 
    //                 std::thread thread(HandleClient);
    //                 thread.join();
    //             }
    //         }
    //     }
    // }
    // return 0;
// }

