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
using namespace std;

// 使用fork实现并发 获取服务器时间

char buf[1024];
// 子进程任务
void doit(int fd){
    time_t ticks = time(NULL);
    snprintf(buf, 1024, "%.24s\r\n", ctime(&ticks));
    write(fd, buf, sizeof(buf));
}
int main(){
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_port = htons(8081);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    
    pid_t pid;

    // 绑定
    bind(serverfd, (struct sockaddr *)&addr, sizeof(addr));

    // 监听
    listen(serverfd, 128);
    int clientfd;
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    
    while(1){
        clientfd = accept(serverfd,(struct sockaddr *)&clientAddr, &len);
        pid = fork();
        // 子进程进行处理
        if(pid == 0){   
            // 子进程关闭监听套接字
            close(serverfd);    
            // 子进程的任务 调用函数doit()
            doit(clientfd);
            // 子进程关闭套接字
            close(clientfd);
            // 子进程退出
            exit(0);
        }
        // 父进程工作
        else if(pid > 0){
            // 关闭连接
            close(clientfd);
            pid_t wpid;
            // 回收子进程
            while((wpid = waitpid(pid, NULL, WNOHANG)) > 0){
                cout<<"回收成功"<<endl;
            }
        }
    }
    return 0;
}