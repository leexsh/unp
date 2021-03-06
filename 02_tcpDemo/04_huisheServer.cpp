#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/select.h>
#include <time.h>
#include <poll.h>
#include <sys/epoll.h>
#include <iostream>

void sig_child(int singno){
    pid_t pid;
    int stat;
    while((pid = waitpid(-1,&stat, WNOHANG)) > 0){
        printf("child %d terminated \n", pid);
    }
    return;
}


// 子进程处理函数 回显
void str_echo(int fd){
    ssize_t n;
    char buf[1024];
    again:
        while((n = read(fd, buf, 1024)) > 0){
            write(fd, buf, 1024);
            if(n < 0 && errno == EINTR){
                goto again;
            }
            else if(n < 0){
                perror("server str_echo error\n");
            }
        }
}


// 1.原始的采用fork()的多进程并发服务器模型main函数

// int main(){
//     // 创建套接字
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     //  结构体
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(8090);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     // 进程ID
//     pid_t pid;

//     // 绑定
//     bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
//     // 监听
//     listen(sockfd, 128);
//     signal(SIGCHLD, sig_child);
//     struct sockaddr_in clientAddr;
//     socklen_t len = sizeof(clientAddr);
//     // 处理
//     while(1){
//         int clientfd;
//         // 连接
//         // 信号处理时 会被父进程捕获 由于父进程堵塞于此 内核可能就会返回一个EINTR的错误
//         if((clientfd = accept(sockfd, (struct sockaddr *)&clientAddr, &len))<0){
//             // 为了防止父进程无法处理EINTR错误 而结束堵塞
//             if(errno == EINTR){
//                 continue;
//             }
//             else
//             {
//                 perror("accept error");
//             }
//         }
//         // 创建进程
//         pid = fork();
//         // 子进程
//         if(pid == 0){
//             // 关闭监听套接字
//             close(sockfd);
//             // 子进程核心处理
//             str_echo(clientfd);
//             exit(0);
//         }else if(pid > 0){
//             // 父进程
//             // 关闭连接套接字
//             close(clientfd);
//         }
//     }

// }


// 2.采用I/O复用的select模型的服务器模型main函数
// int main(){
//     // 创建套接字
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     //  结构体
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(8090);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     // 进程IDgit
//     pid_t pid;

//     // 绑定
//     bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
//     // 监听
//     listen(sockfd, 128);
//     signal(SIGCHLD, sig_child);

//     // ----select操作----
//     // select集合
//     fd_set oldSet, newSet;
//     int imax = -1;   // 记录clientfd数组中用到的最大位置
//     // 清空
//     FD_ZERO(&oldSet);
//     // 客户端描述符集合并进行初始化 客户端最多有1023个描述符 因为最大监听集合就是1024
//     int clientfd[1023]; // 1023
//     for(int i = 0; i < 1023; ++i){
//         clientfd[i] = -1;
//     }
//     // 将服务器监听描述符加入到select
//     FD_SET(sockfd, &oldSet);
//     // 为什么最大描述符的数目是sockfd+1， 因为此时监听集合中只有监听sockfd一个描述符
//     int maxfd = sockfd;  //最大描述符数目 
//     struct sockaddr_in clientAddr;
//     socklen_t len = sizeof(clientAddr);
//     // 处理
//     int i;
//     while(1){
//         // 连接的客户端描述符
//         int connfd;
//         // 传入和传出集合分离 因为select后 所有未就绪的集合都要清0 不能破坏原来集合中关心的位置 所以用一个新的集合
//         newSet = oldSet;
//         // select函数
//         int ready = select(maxfd + 1, &newSet, NULL, NULL, NULL);

//         // 判断就绪集合
//         // 服务器准备就绪
//         if(FD_ISSET(sockfd, &newSet)){
//             // 接收客户端的连接
//             connfd = accept(sockfd, (struct sockaddr *)&clientAddr, &len);
//             // 加入到监听集合
//             FD_SET(connfd, &oldSet);
//             // 保存客户端的描述符
            
//             for(i = 0; i < 1023; ++i){
//                 if(clientfd[i] == -1){
//                     clientfd[i] = connfd;
//                     break;
//                 }
//             }
//             // 记录用到的connfd数目
//             if(imax < i){
//                 imax = i;
//             }
//             // 更新最大文件描述符
//             if(connfd > maxfd){
//                 maxfd = connfd;
//             }
//             // 只有客户准备就绪 继续去select
//             if(--ready <= 0){
//                 continue;
//             }
//         }
//         // 客户端就绪
//         for(i = 0; i <= imax; ++i){
//             // 小于0的文件描述符 不管
//             if(clientfd[i] < 0){
//                 continue;
//             }
//             // 准备就绪的客户端文件描述符
//             if(FD_ISSET(clientfd[i], &newSet)){
//                 int n;
//                 char buf[1500];
//                 // 读到的数据大于0
//                 if((n = read(clientfd[i], buf, sizeof(buf))) > 0){
//                     // 回写客户端
//                     write(clientfd[i], buf, n);
//                 }else if(n == 0)
//                 {
//                     // 没读到数据
//                     close(clientfd[i]);
//                     // 请出集合
//                     FD_CLR(clientfd[i], &oldSet);
//                     // 数组置为-1
//                     clientfd[i] = -1;
//                 }
//                 // 处理完毕
//                 if(--ready <= 0){
//                     break;
//                 }
//             }
//         }
     
//     }

// }


// // 3.采用I/O复用的poll模型的服务器模型main函数
// int main(){
//     // 创建套接字
//     int sockfd = socket(AF_INET, SOCK_STREAM, 0);

//     //  结构体
//     struct sockaddr_in addr;
//     addr.sin_family = AF_INET;
//     addr.sin_port = htons(8090);
//     addr.sin_addr.s_addr = htonl(INADDR_ANY);

//     // 进程ID
//     pid_t pid;

//     // 绑定
//     bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
//     // 监听
//     listen(sockfd, 128);
//     signal(SIGCHLD, sig_child);

//     // ---poll操作----
//     // poll结合
//     struct pollfd arrfd[4096];
//     arrfd[0].fd = sockfd;
//     // 监听服务器端接口
//     arrfd[0].events = POLLIN;
//     // 初始化
//     for(int i = 1; i < 4096; ++i){
//         arrfd[i].fd = -1;
//     }
//     // 最大的客户端描述符数目
//     int imax = 0;
//     int i;
//     while(1){
        
//         // 客户端连接描述符
//         int connfd;
//         struct sockaddr_in caddr;
//         socklen_t len = sizeof(caddr);
//         // poll函数 返回就绪数目 poll(监听集合，最大的监听数目，事件)
//         int ready = poll(arrfd,imax + 1, -1);
//         // 服务器监听端口就绪
//         if(arrfd[0].revents & POLLIN){
//             // 客户端连接
//             connfd = accept(sockfd, (struct sockaddr*)&caddr, &len);
//             // 保存客户端的描述符
           
//             for(i = 1;i<4096; ++i){
//                 if(arrfd[i].fd == -1){
//                     arrfd[i].fd = connfd;
//                     break;
//                 }
//             }
//             arrfd[i].events = POLLIN;
//             // 更新当前的最大描述符数目
//             if(i > imax){
//                 imax = i;
//             }
//             if(--ready <= 0){
//                 continue;
//             }
//         }
//         // 客户端准备就绪
//         for(i = 1; i<= imax; ++i){
//             if(arrfd[i].fd == -1){
//                 continue;
//             }
//             int n;
//             char buf[1500];
//             // 客户端有读事件
//             if(arrfd[i].revents & POLLIN){
//                 // 读取内容大于0 回显给客户端
//                 if((n = read(arrfd[i].fd, buf, 1500)) > 0){
//                     write(arrfd[i].fd, buf, n);
//                 }
//                 // 关闭客户端 并回复默认的fd
//                 else if(n == 0){
//                     close(arrfd[i].fd);
//                     arrfd[i].fd = -1;
//                 }
//                 if(--ready <= 0){
//                     break;
//                 }
//             }
//         }
     
//     }

// }

// 3.采用I/O复用的epoll模型的服务器模型main函数
int main(){
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //  结构体
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8090);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 进程ID
    pid_t pid;

    // 绑定
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    // 监听
    listen(sockfd, 128);
    signal(SIGCHLD, sig_child);

    // ---epoll操作----
    // epoll事件的就绪数组
    struct epoll_event readyArr[1000];
    // 监听集合
    int epoll_fd = epoll_create(1000);
    
    // 将服务器接收端口设置为监听项
    struct epoll_event epollTemp;
    epollTemp.events = EPOLLIN;
    epollTemp.data.fd = sockfd;

    // 加入到监听集合
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sockfd,&epollTemp);
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);
    while(1){
        int ready = epoll_wait(epoll_fd, readyArr,1000,-1);
        while(ready){
            int i = 0;
            // 客户端来连接服务器
            if(readyArr[i].data.fd == sockfd){
                int connfd = accept(sockfd, (struct sockaddr*)&caddr, &len);
                if(connfd > 0){
                    std::cout<<"连接成功"<<std::endl;
                }
                // 创建连接接入到监听集合
                epollTemp.events = EPOLLIN;
                epollTemp.data.fd = connfd;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connfd, &epollTemp);
            
            }
            else{
                // 回显客户端
                char buf[1500];
                int n ;
                n = read(readyArr[i].data.fd,buf, 1500);
                if(n > 0){
                    write(readyArr[i].data.fd, buf,n);
                }
                else{
                    close(readyArr[i].data.fd);
                    epoll_ctl(epoll_fd,EPOLL_CTL_DEL, readyArr[i].data.fd, NULL);
                }
            }
            --ready;
            ++i;
        }
    }
    return 0;
}