#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#define PORT 5375
#define SERVER_IP "127.0.0.1"
#define BUFSIZE 4096
#define DURATION 10   // 전송 지속 시간 (초)

/*
 * 사용법: ./tcp_client <전송속도(bytes/s)>
 * 예시  : ./tcp_client 500
 *         ./tcp_client 1000
 *         ./tcp_client 2000
 */
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("사용법: %s <전송속도(bytes/s)>\n", argv[0]);
        printf("예시  : %s 500\n", argv[0]);
        exit(1);
    }

    int send_rate = atoi(argv[1]);   // bytes per second
    if (send_rate <= 0) {
        printf("전송속도는 양수여야 합니다.\n");
        exit(1);
    }

    int sock_fd;
    struct sockaddr_in server_addr;
    char buff[BUFSIZE];
    memset(buff, 'A', BUFSIZE);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("connect 실패\n");
        exit(1);
    }
    printf("서버 연결 완료! 전송속도: %d bytes/s, 지속시간: %d초\n",
           send_rate, DURATION);

    long long total_sent = 0;
    struct timespec start, now, loop_start, loop_end;
    long elapsed_ns;

    clock_gettime(CLOCK_MONOTONIC, &start);

    /*
     * 매 루프마다 1초 간격으로 send_rate bytes를 전송한다.
     * chunk 크기를 send_rate로 제한하여 정확한 속도 제어.
     */
    for (int sec = 0; sec < DURATION; sec++) {
        clock_gettime(CLOCK_MONOTONIC, &loop_start);

        int remaining = send_rate;
        while (remaining > 0) {
            int chunk = (remaining < BUFSIZE) ? remaining : BUFSIZE;
            int bytes_sent = send(sock_fd, buff, chunk, 0);
            if (bytes_sent < 0) {
                printf("send 실패 (sec=%d)\n", sec);
                goto done;
            }
            total_sent += bytes_sent;
            remaining  -= bytes_sent;
        }

        /* 1초에 딱 맞게 남은 시간 sleep */
        clock_gettime(CLOCK_MONOTONIC, &loop_end);
        elapsed_ns = (loop_end.tv_sec  - loop_start.tv_sec)  * 1000000000LL
                   + (loop_end.tv_nsec - loop_start.tv_nsec);
        long sleep_ns = 1000000000LL - elapsed_ns;
        if (sleep_ns > 0) {
            struct timespec ts = { .tv_sec = 0, .tv_nsec = sleep_ns };
            nanosleep(&ts, NULL);
        }

        printf("[%2d초] 누적 전송: %lld bytes\n", sec + 1, total_sent);
    }

done:
    clock_gettime(CLOCK_MONOTONIC, &now);
    double elapsed = (now.tv_sec  - start.tv_sec)
                   + (now.tv_nsec - start.tv_nsec) / 1e9;
    double throughput_kBps = total_sent / elapsed / 1000.0;

    printf("\n===== TCP 전송 결과 =====\n");
    printf("전송속도 설정   : %d bytes/s\n", send_rate);
    printf("총 전송 바이트  : %lld bytes\n", total_sent);
    printf("경과 시간        : %.3f 초\n", elapsed);
    printf("Throughput (TX)  : %.2f kbps\n", throughput_kBps);
    printf("=========================\n");

    close(sock_fd);
    return 0;
}