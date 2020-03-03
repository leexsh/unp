#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

int main(){

    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_family     = AF_INET;
    addr.sin_port       = htons(8081); 
    inet_pton(AF_INET, "101.37.118.181", &addr.sin_addr);

    // 创建套接字
    int  sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // 连接
    connect(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    int n;
    char buf[1024];
    while((n = read(sockfd,buf,1024)) > 0){
        buf[n] = 0;
        if(fputs(buf, stdout) == EOF){
            perror("error");
        }
    }
    if(n < 0){
        perror("read error");
    }

    // 连接后 保持连接 测试并发
    while(1);
 
    return 0;
}