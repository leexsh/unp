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
#include <iostream>
using namespace std;
void udp_echo(int fd, struct sockaddr* addr, socklen_t addrlen){
    int n;
    // socklen_t len;
    char buf[1500];
    while(1){
        // len = addrlen;
        n = recvfrom(fd,buf,1500,0,addr,&addrlen);
        sendto(fd, buf,n,0,addr,addrlen);
    }
}
int main(){
    // 创建套接字
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr, caddr;
    addr.sin_port = htons(9080);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));

    // 回显
    udp_echo(sockfd, (struct sockaddr *)&caddr, sizeof(caddr));

    return 0;
}