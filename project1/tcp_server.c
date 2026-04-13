#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>

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
    printf("서버 시작, 포트 %d에서 대기 중...\n", PORT);

    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(client_addr.sin_addr));

    // 데이터 수신 후 삭제
    int bytes_received;
    while ((bytes_received = recv(client_fd, buff, BUFSIZE, 0)) > 0) {
        printf("수신: %d bytes\n", bytes_received);
        memset(buff, 0, BUFSIZE);
    }

    printf("클라이언트 연결 종료\n");

    close(client_fd);
    close(server_fd);
    return 0;
}