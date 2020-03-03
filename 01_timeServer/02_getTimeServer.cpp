#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
using namespace std;

int main(){
    struct sockaddr_in addr;
    bzero(&addr, sizeof(addr));
    addr.sin_port = htons(8081);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int serverfd = socket(AF_INET, SOCK_STREAM, 0);
    
    // 绑定
    bind(serverfd, (struct sockaddr *)&addr, sizeof(addr));

    // 监听
    listen(serverfd, 128);
    int clientfd;
    struct sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);
    char buf[1024];
    while(1){
        clientfd = accept(serverfd,(struct sockaddr *)&clientAddr, &len);
        time_t ticks = time(NULL);
        snprintf(buf, 1024, "%.24s\r\n", ctime(&ticks));
        write(clientfd, buf, sizeof(buf));
        close(clientfd);
    }
    return 0;
}