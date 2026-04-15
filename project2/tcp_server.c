#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

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

    int opt = 1;
    setsockopt(srvfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

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
    printf("TCP 서버 시작, 포트 %d에서 대기 중...\n", PORT);

    clifd = accept(srvfd, (struct sockaddr*)&cliaddr, &clilen);
    if (clifd < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(cliaddr.sin_addr));

    long long totalbytes = 0;
    int bytes_received;
    struct timespec start, now;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while ((bytes_received = recv(clifd, buff, BUFSIZE, 0)) > 0) {
        totalbytes += bytes_received;

        clock_gettime(CLOCK_MONOTONIC, &now);
        double elapsed = (now.tv_sec - start.tv_sec)
                       + (now.tv_nsec - start.tv_nsec) / 1e9;

        printf("[%.2f초] 수신: %d bytes | 누적: %lld bytes\n",
               elapsed, bytes_received, totalbytes);

        memset(buff, 0, BUFSIZE);
    }

    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = (now.tv_sec - start.tv_sec)
                   + (now.tv_nsec - start.tv_nsec) / 1e9;
    double tputBps  = totalbytes / elapsed;
    double tputkBps = tputBps / 1000.0;

    printf("\n===== TCP Throughput 측정 결과 (서버 기준) =====\n");
    printf("총 수신 바이트  : %lld bytes\n", totalbytes);
    printf("경과 시간        : %.3f 초\n", elapsed);
    printf("Throughput (RX)  : %.2f Bytes/s (%.2f kBytes/s)\n", tputBps, tputkBps);
    printf("================================================\n");

    close(clifd);
    close(srvfd);
    return 0;
}