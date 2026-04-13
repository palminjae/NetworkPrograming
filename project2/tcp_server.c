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
    int server_fd;
    struct sockaddr_in server_addr;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buff[BUFSIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("bind 실패\n");
        exit(1);
    }

    if (listen(server_fd, 1) < 0) {
        printf("listen 실패\n");
        exit(1);
    }
    printf("TCP 서버 시작, 포트 %d에서 대기 중...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(client_addr.sin_addr));

    long long total_bytes = 0;
    int bytes_received;

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    while ((bytes_received = recv(client_fd, buff, BUFSIZE, 0)) > 0) {
        total_bytes += bytes_received;
        memset(buff, 0, BUFSIZE);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec)
                   + (end.tv_nsec - start.tv_nsec) / 1e9;
    double throughput_Bps  = total_bytes / elapsed;
    double throughput_kBps = throughput_Bps / 1000.0;

    printf("\n===== TCP Throughput 측정 결과 =====\n");
    printf("총 수신 바이트 : %lld bytes\n", total_bytes);
    printf("경과 시간       : %.3f 초\n", elapsed);
    printf("Throughput      : %.2f bps (%.2f kbps)\n", throughput_Bps, throughput_kBps);
    printf("====================================\n");

    close(client_fd);
    close(server_fd);
    return 0;
}