#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <sys/select.h>
using namespace std;

// 1.原始版本
// void str_cli(FILE *fp,int fd){
//     char buf[1500];
//     int len;
//     while(fgets(buf, sizeof(buf), fp)!= NULL){
//         write(fd, buf, strlen(buf));
//         cout<<"客户端发送内容:"<<buf;
//         bzero(buf, sizeof(buf));
//         read(fd, buf, sizeof(buf));
//         cout<<"服务器返回内容:"<<buf;
//         bzero(buf, sizeof(buf));
        
//     }
// }

// 2.select版本1
// void str_cli(FILE *fp,int fd){
//     char buf[1024];
//     bzero(buf, 1024);
//     // select集合
//     fd_set oldSet;
//     // 最大描述符
//     int maxfd;
//     // 清空
//     FD_ZERO(&oldSet);
//     while(1){
//         // 将读加入到select
//         FD_SET(fileno(fp), &oldSet);
//         // 将发送加入到select
//         FD_SET(fd, &oldSet);
//         // 获取最大描述符
//         maxfd = max(fileno(fp), fd) + 1;
//         // 获取就绪文件描述符
//         int ready = select(maxfd, &oldSet, NULL, NULL, NULL);
        
//         // 向服务器发数据
//         if(FD_ISSET(fileno(fp), &oldSet)){
//              if((fgets(buf, sizeof(buf), fp)) != NULL){
//                 write(fd, buf, strlen(buf));
//                 cout<<"客户端发送内容:"<<buf;
//                 bzero(buf, sizeof(buf));
//             }
//         }

//         // 接收服务器返回的数据
//         if(FD_ISSET(fd, &oldSet)){
//             if(read(fd, buf, sizeof(buf)) > 0){
//                 cout<<"服务器返回内容:"<<buf;
//                 bzero(buf, sizeof(buf));
//             }  
//         }
//     }
// }

// 3.select版本2 加入了shutdown()函数
void str_cli(FILE *fp,int fd){
    char buf[1024];
    bzero(buf, 1024);
    // select集合
    fd_set oldSet;
    // 最大描述符
    int maxfd, stdineof = 0;
    // 清空
    FD_ZERO(&oldSet);
    while(1){
        // 将读加入到select
        if(stdineof == 0)
            FD_SET(fileno(fp), &oldSet);
        // 将发送加入到select
        FD_SET(fd, &oldSet);
        // 获取最大描述符
        maxfd = max(fileno(fp), fd) + 1;
        // 获取就绪文件描述符
        int ready = select(maxfd, &oldSet, NULL, NULL, NULL);
        
        // 向服务器发数据
        if(FD_ISSET(fileno(fp), &oldSet)){
            if((read(fileno(fp), buf, 1024)) == 0){
                stdineof = 1;
                shutdown(fd, SHUT_WR);
                FD_CLR(fileno(fp), &oldSet);
                continue;
            }
            write(fd, buf, strlen(buf));
            cout<<"客户端发送内容:"<<buf;
            bzero(buf, sizeof(buf));
        }

        // 接收服务器返回的数据
        if(FD_ISSET(fd, &oldSet)){
            if(read(fd, buf, sizeof(buf)) == 0){
                if(stdineof == 1){
                    return;
                }
                else
                {
                    perror("error");
                }
            }  
            cout<<"服务器返回内容:"<<buf;
                bzero(buf, sizeof(buf));
        }
    }
}
int main(){

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8090);
    inet_pton(AF_INET, "101.37.118.181", &addr.sin_addr);

    // connect
    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    str_cli(stdin, sockfd);
    exit(0);



    // 多个 sockfd
    // 客户端与服务器建立五个连接
    // int sockfd[5], i;
    // for(i = 0; i < 5; i++){
    //     sockfd[i] = socket(AF_INET, SOCK_STREAM, 0);

    //     struct sockaddr_in addr;
    //     bzero(&addr, sizeof(addr));
    //     addr.sin_family = AF_INET;
    //     addr.sin_port = htons(8090);
    //     inet_pton(AF_INET, "101.37.118.181", &addr.sin_addr);

    //     // connect
    //     connect(sockfd[i], (struct sockaddr *)&addr, sizeof(addr));
    // }

    // // 仅有一个调用回显函数
    // str_cli(sockfd[0]);

    // exit(0);
}