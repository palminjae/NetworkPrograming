#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 4732
#define BUFSIZE 4096

int main() {
    int srvfd;
    struct sockaddr_in srvaddr;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    char buff[BUFSIZE];

    srvfd = socket(AF_INET, SOCK_DGRAM, 0);
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
    printf("UDP 서버 시작, 포트 %d에서 대기 중...\n", PORT);

    int bytes_received;
    while ((bytes_received = recvfrom(srvfd, buff, BUFSIZE, 0,
                            (struct sockaddr*)&cliaddr, &clilen)) > 0) {
        printf("수신: %d bytes | 클라이언트: %s\n",
               bytes_received, inet_ntoa(cliaddr.sin_addr));
        memset(buff, 0, BUFSIZE);
    }

    printf("수신 종료\n");
    close(srvfd);
    return 0;
}
