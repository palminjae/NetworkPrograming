#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

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
        exit(1);
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = INADDR_ANY;
    srvaddr.sin_port = htons(PORT);

    if (bind(srvfd, (struct sockaddr*)&srvaddr, sizeof(srvaddr)) < 0) {
        exit(1);
    }
    printf("UDP 서버 시작, 포트 %d에서 대기 중...\n", PORT);

    long long total = 0;
    int received;
    struct timespec start, end;
    int started = 0;

    while ((received = recvfrom(srvfd, buff, BUFSIZE, 0,
                                  (struct sockaddr*)&cliaddr, &clilen)) > 0) {

    if (!started) { // 첫 수신을 기점으로 시간 측정을 시작
        clock_gettime(CLOCK_MONOTONIC, &start);
        printf("수신 시작: 클라이언트 %s\n", inet_ntoa(cliaddr.sin_addr));
        started = 1;
    }

    if (received == 3 && strncmp(buff, "END", 3) == 0) { // 종료 신호 받기 tcp에는 있지만 udp에는 없으므로 recvfrom으로 종료 신호 받도록
        clock_gettime(CLOCK_MONOTONIC, &end); 
        printf("종료 신호 수신 → 전송 종료\n");
        break;
    }

    total += received;
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsedtime = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("[%.2f초] 수신: %d bytes | 누적: %lld bytes\n",
           elapsedtime, received, total);
    memset(buff, 0, BUFSIZE);
}

    if (!started) {
        printf("수신된 데이터가 없습니다.\n");
    } else {
        double elapsedtime = (end.tv_sec - start.tv_sec)
                       + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
        double tputBps  = total / elapsedtime;
        double tputkBps = tputBps / 1000.0;

        printf("\n===== UDP Throughput 측정 결과 (서버 기준) =====\n");
        printf("총 수신 바이트  : %lld bytes\n", total);
        printf("경과 시간        : %.3f 초\n", elapsedtime);
        printf("Throughput (RX)  : %.2f Bytes/s (%.2f kBytes/s)\n", tputBps, tputkBps);
    }

    close(srvfd);
    return 0;
}