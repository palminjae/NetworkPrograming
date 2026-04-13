#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5375
#define SERVER_IP "127.0.0.1"
#define BUFSIZE 4096

int main() {
    int sock_fd;
    struct sockaddr_in server_addr;
    char buff[BUFSIZE];

    // 전송할 더미 데이터로 버퍼 채우기
    memset(buff, 'A', BUFSIZE);

    // 소켓 생성
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // 서버에 연결
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("connect 실패\n");
        exit(1);
    }
    printf("서버 연결 완료!\n");

    // 데이터 전송 (10번 반복)
    int bytes_sent = send(sock_fd, buff, BUFSIZE, 0);
    if (bytes_sent < 0) {
        printf("send 실패\n");
    }  
    else {
        printf("전송 완료! %d bytes 전송\n", bytes_sent);
    }
    // 소켓 닫기
    close(sock_fd);
    return 0;
}