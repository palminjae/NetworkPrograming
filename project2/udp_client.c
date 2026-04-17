#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 4732
#define SERVER_IP "127.0.0.1"
#define BUFSIZE 4096
#define DURATION 100

int main(int argc, char *argv[]) {
    if (argc < 2) { // 이거도 전송속도 -> 인자로 받도록
        printf("사용법: %s <전송속도(bytes/s)>\n", argv[0]);
        printf("예시  : %s 500\n", argv[0]);
        exit(1);
    }

    int sendrate = atoi(argv[1]);
    if (sendrate <= 0) {
        printf("전송속도는 양수여야 합니다.\n");
        exit(1);
    }

    int skfd;
    struct sockaddr_in srvaddr;
    char buff[BUFSIZE];
    memset(buff, 'A', BUFSIZE);

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (skfd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    memset(&srvaddr, 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    srvaddr.sin_port = htons(PORT);

    printf("UDP 전송 시작! 전송속도: %d bytes/s, 지속시간: %d초\n", sendrate, DURATION);

    long long total = 0;
    struct timespec start, now, loopstart, loopend;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int sec = 0; sec < DURATION; sec++) { // duration 동안 반복
        clock_gettime(CLOCK_MONOTONIC, &loopstart);

        int remaining = sendrate;
        while (remaining > 0) {
            int chunk = (remaining < BUFSIZE) ? remaining : BUFSIZE;
            int sent = sendto(skfd, buff, chunk, 0,
                                    (struct sockaddr*)&srvaddr, sizeof(srvaddr));
            if (sent < 0) {
                printf("sendto 실패 (sec=%d)\n", sec);
                break;
            }
            total += sent;
            remaining -= sent;
        }

        clock_gettime(CLOCK_MONOTONIC, &loopend); // 1초에 맞춰 nanosleep으로 속도 제어
        long elapsedns = (loopend.tv_sec  - loopstart.tv_sec)  * 1000000000LL
                       + (loopend.tv_nsec - loopstart.tv_nsec);
        long sleepns = 1000000000LL - elapsedns;
        if (sleepns > 0) {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = sleepns };
            nanosleep(&ts, NULL);
        }

        printf("[%2d초] 누적 전송: %lld bytes\n", sec + 1, total);
    }

    sendto(skfd, "END", 3, 0, (struct sockaddr*)&srvaddr, sizeof(srvaddr)); // udp는 연결이 없으므로 sendto로 종료 신호 전송
    printf("종료 신호 전송 완료\n");

    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsedtime = (now.tv_sec  - start.tv_sec)
                   + (now.tv_nsec - start.tv_nsec) / 1e9;
    double tputBps  = total / elapsedtime;
    double tputkBps = tputBps / 1000.0;

    printf("\n===== UDP 전송 결과 (클라이언트 기준) =====\n");
    printf("전송속도 설정   : %d Bytes/s\n", sendrate);
    printf("총 전송 바이트  : %lld bytes\n", total);
    printf("경과 시간        : %.3f 초\n", elapsedtime);
    printf("Throughput (TX)  : %.2f Bytes/s (%.2f kBytes/s)\n", tputBps, tputkBps);
    close(skfd);
    return 0;
}