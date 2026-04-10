#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>

#define PORT 4732
#define SERVER_IP "127.0.0.1"
#define DURATION 30

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <send_rate_bytes_per_sec>\n", argv[0]);
        fprintf(stderr, "Example: %s 500\n", argv[0]);
        exit(1);
    }

    int send_rate = atoi(argv[1]);
    if (send_rate != 500 && send_rate != 1000 && send_rate != 2000) {
        fprintf(stderr, "send_rate must be 500, 1000, or 2000\n");
        exit(1);
    }

    int sock_fd;
    struct sockaddr_in server_addr;

    // 소켓 생성
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        perror("socket");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    printf("[UDP Client] Target: %s:%d\n", SERVER_IP, PORT);
    printf("[UDP Client] Send rate: %d bytes/sec, Duration: %d sec\n", send_rate, DURATION);

    // 전송할 데이터 준비
    char *send_buf = malloc(send_rate);
    if (!send_buf) {
        perror("malloc");
        exit(1);
    }
    memset(send_buf, 'A', send_rate);

    long long total_sent = 0;
    struct timeval start, now, interval_start;
    gettimeofday(&start, NULL);

    for (int sec = 0; sec < DURATION; sec++) {
        gettimeofday(&interval_start, NULL);

        int sent = sendto(sock_fd, send_buf, send_rate, 0,
                          (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (sent < 0) {
            perror("sendto");
            break;
        }
        total_sent += sent;

        gettimeofday(&now, NULL);
        double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1e6;
        printf("[UDP Client] Sent: %lld bytes | Elapsed: %.2f s\n", total_sent, elapsed);

        // 1초 간격 유지
        double send_time = (now.tv_sec - interval_start.tv_sec) +
                           (now.tv_usec - interval_start.tv_usec) / 1e6;
        double sleep_time = 1.0 - send_time;
        if (sleep_time > 0) {
            struct timespec ts;
            ts.tv_sec = (time_t)sleep_time;
            ts.tv_nsec = (long)((sleep_time - ts.tv_sec) * 1e9);
            nanosleep(&ts, NULL);
        }
    }

    // 종료 신호 전송
    sendto(sock_fd, "END", 3, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));

    free(send_buf);

    gettimeofday(&now, NULL);
    double total_elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1e6;
    printf("[UDP Client] === Final Result ===\n");
    printf("[UDP Client] Total bytes sent: %lld\n", total_sent);
    printf("[UDP Client] Total time: %.2f s\n", total_elapsed);
    printf("[UDP Client] Average send rate: %.2f Bps\n", total_sent / total_elapsed);

    close(sock_fd);
    return 0;
}
