#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 4732
#define SERVER_IP "127.0.0.1"
#define BUFSIZE 4096

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buff[BUFSIZE];

    memset(buff, 'A', BUFSIZE);  // 전송할 더미 데이터

    // 소켓 생성 (SOCK_DGRAM = UDP!)
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    int total_sent = 0;
    for (int i = 0; i < 10; i++) {
        int bytes_sent = sendto(sock_fd, buff, BUFSIZE, 0,
                        (struct sockaddr*)&server_addr, sizeof(server_addr));
        if (bytes_sent < 0) {
            printf("sendto 실패\n");
            break;
        }
        total_sent += bytes_sent;
        printf("전송: %d bytes | 총 전송: %d bytes\n", bytes_sent, total_sent);
        sleep(1);
    }

    printf("전송 완료! 총 %d bytes 전송\n", total_sent);

    close(sock_fd);

    return 0;
}