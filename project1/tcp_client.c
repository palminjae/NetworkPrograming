#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5375
#define SERVER_IP "127.0.0.1"
#define BUFSIZE 4096

int main() {
    int skfd;
    struct sockaddr_in srvaddr;
    char buff[BUFSIZE];

    memset(buff, 'A', BUFSIZE);

    skfd = socket(AF_INET, SOCK_STREAM, 0);
    if (skfd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    srvaddr.sin_port = htons(PORT);

    if (connect(skfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
        printf("connect 실패\n");
        exit(1);
    }
    printf("서버 연결 완료!\n");

    int bytes_sent = send(skfd, buff, BUFSIZE, 0);
    if (bytes_sent < 0) {
        printf("send 실패\n");
    }  
    else {
        printf("전송 완료! %d bytes 전송\n", bytes_sent);
    }
    close(skfd);
    return 0;
}
