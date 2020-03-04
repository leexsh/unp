#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

void udp_cli(int sockfd, struct sockaddr *addr, socklen_t len){
    int n;
    char buf[1500];
    bzero(buf, 1500);
    while(fgets(buf,1500,stdin) != NULL){
        sendto(sockfd,buf, strlen(buf),0,addr,len);
        cout<<"客户端发送:"<<buf;
        bzero(buf, 1500);
        n = recvfrom(sockfd, buf,1500,0,addr,&len);
        cout<<"服务器回显:"<<buf;
        bzero(buf,1500);
    }
}
// // 1.普通udp
// int main(){

//     // 套接字
//     int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

//     // 
//     struct sockaddr_in addr;
//     addr.sin_port = htons(9080);
//     addr.sin_family = AF_INET;
//     inet_pton(AF_INET, "101.37.118.181", &addr.sin_addr);

//     udp_cli(sockfd, (struct sockaddr *)&addr, sizeof(addr));
//     // return 0;
//     exit(0);
// }

