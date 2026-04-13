#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5375
#define BUFSIZE 4096

int main() {
    int srvfd;
    struct sockaddr_in srvaddr;
    int clifd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    char buff[BUFSIZE];

    srvfd = socket(AF_INET, SOCK_STREAM, 0);
    if (srvfd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(PORT);

    if (bind(srvfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
        printf("bind 실패\n");
        exit(1);
    }

    if (listen(srvfd, 1) < 0) {
        printf("listen 실패\n");
        exit(1);
    }
    printf("서버 시작, 포트 %d에서 대기 중...\n", PORT);

    clifd = accept(srvfd, (struct sockaddr*)&cliaddr, &clilen);
    if (clifd < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(cliaddr.sin_addr));

    int bytes_received;
    while ((bytes_received = recv(clifd, buff, BUFSIZE, 0)) > 0) {
        printf("수신: %d bytes\n", bytes_received);
        memset(buff, 0, BUFSIZE);
    }

    printf("클라이언트 연결 종료\n");

    close(clifd);
    close(srvfd);
    return 0;
}
