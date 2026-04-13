#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 5376
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

    long long totalbytes = 0;
    int bytes_received;
    struct timespec start, end;
    int started = 0;

    while ((bytes_received = recvfrom(srvfd, buff, BUFSIZE, 0,
                             (struct sockaddr*)&cliaddr, &clilen)) > 0) {
        if (!started) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            printf("수신 시작: 클라이언트 %s\n", inet_ntoa(cliaddr.sin_addr));
            started = 1;
        }
        totalbytes += bytes_received;
        memset(buff, 0, BUFSIZE);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1e9;
    double tputBps  = totalbytes / elapsed;
    double tputkBps = tputBps / 1000.0;

    printf("\n===== UDP Throughput 측정 결과 =====\n");
    printf("총 수신 바이트 : %lld bytes\n", totalbytes);
    printf("경과 시간       : %.3f 초\n", elapsed);
    printf("Throughput      : %.2f bps (%.2f kbps)\n", tputBps, tputkBps);
    printf("====================================\n");

    close(srvfd);
    return 0;
}