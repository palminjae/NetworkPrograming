#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <sys/select.h>

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

    long long totalbytes = 0;
    int bytes_received;
    struct timespec start, end;
    int started = 0;

    while (1) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(srvfd, &readfds);
        struct timeval tv = { .tv_sec = 3, .tv_usec = 0 };

        int ret = select(srvfd + 1, &readfds, NULL, NULL, &tv);
        if (ret == 0) {
            // 타임아웃: 클라이언트가 전송 종료한 것으로 판단
            printf("수신 타임아웃 → 전송 종료로 판단\n");
            break;
        }
        if (ret < 0) break;

        bytes_received = recvfrom(srvfd, buff, BUFSIZE, 0,
                                  (struct sockaddr*)&cliaddr, &clilen);
        if (bytes_received <= 0) break;

        if (!started) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            printf("수신 시작: 클라이언트 %s\n", inet_ntoa(cliaddr.sin_addr));
            started = 1;
        }

        // 종료 신호 확인 (totalbytes에 더하기 전에 체크)
        if (bytes_received == 3 && strncmp(buff, "END", 3) == 0) {
            clock_gettime(CLOCK_MONOTONIC, &end); 
            printf("종료 신호 수신 → 전송 종료\n");
            break;
        }

        totalbytes += bytes_received;
        clock_gettime(CLOCK_MONOTONIC, &end);

        double elapsed = (end.tv_sec - start.tv_sec)
                       + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("[%.2f초] 수신: %d bytes | 누적: %lld bytes\n",
               elapsed, bytes_received, totalbytes);
        memset(buff, 0, BUFSIZE);
    }

    if (!started) {
        printf("수신된 데이터가 없습니다.\n");
    } else {
        double elapsed = (end.tv_sec - start.tv_sec)
                       + (end.tv_nsec - start.tv_nsec) / 1e9;
        double tputBps  = totalbytes / elapsed;
        double tputkBps = tputBps / 1000.0;

        printf("\n===== UDP Throughput 측정 결과 (서버 기준) =====\n");
        printf("총 수신 바이트  : %lld bytes\n", totalbytes);
        printf("경과 시간        : %.8f 초\n", elapsed);
        printf("Throughput (RX)  : %.8f Bytes/s (%.2f kBytes/s)\n", tputBps, tputkBps);
        printf("================================================\n");
    }

    close(srvfd);
    return 0;
}