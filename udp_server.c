#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 4732
#define BUFFER_SIZE 4096

int main() {
    int server_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    // 소켓 생성
    server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(1);
    }

    // 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 바인드
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("[UDP Server] Listening on port %d...\n", PORT);

    // 데이터 수신 및 throughput 측정
    long long total_bytes = 0;
    int bytes_received;
    struct timeval start, now;
    int first = 1;

    while (1) {
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE, 0,
                                  (struct sockaddr*)&client_addr, &client_len);
        if (bytes_received < 0) {
            perror("recvfrom");
            break;
        }

        // "END" 종료 신호 확인
        if (bytes_received >= 3 && strncmp(buffer, "END", 3) == 0) {
            printf("[UDP Server] Received END signal. Stopping.\n");
            break;
        }

        if (first) {
            printf("[UDP Server] First packet from: %s\n", inet_ntoa(client_addr.sin_addr));
            gettimeofday(&start, NULL);
            first = 0;
        }

        total_bytes += bytes_received;

        gettimeofday(&now, NULL);
        double elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1e6;

        if (elapsed > 0) {
            double throughput = (total_bytes * 8.0) / elapsed;
            printf("[UDP Server] Received: %lld bytes | Elapsed: %.2fs | Throughput: %.2f bps (%.2f Bps)\n",
                   total_bytes, elapsed, throughput, throughput / 8.0);
        }

        // 수신 후 데이터 삭제
        memset(buffer, 0, BUFFER_SIZE);
    }

    gettimeofday(&now, NULL);
    double total_elapsed = (now.tv_sec - start.tv_sec) + (now.tv_usec - start.tv_usec) / 1e6;
    printf("[UDP Server] === Final Result ===\n");
    printf("[UDP Server] Total bytes received: %lld\n", total_bytes);
    printf("[UDP Server] Total time: %.2f s\n", total_elapsed);
    if (total_elapsed > 0)
        printf("[UDP Server] Average throughput: %.2f bps (%.2f Bps)\n",
               (total_bytes * 8.0) / total_elapsed, total_bytes / total_elapsed);

    close(server_fd);
    return 0;
}
