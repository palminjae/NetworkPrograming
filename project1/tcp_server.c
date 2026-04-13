#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 5375
#define BUFSIZE 4096

int main() {
    int server_socket_file_descriptor;
    struct sockaddr_in server_address_info;
    int client_socket_file_descriptor;
    struct sockaddr_in client_address_info;
    socklen_t client_address_length = sizeof(client_address_info);
    char buff[BUFSIZE];

    server_socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket_file_descriptor < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    memset(&server_address_info, 0, sizeof(server_address_info));
    server_address_info.sin_family = AF_INET;
    server_address_info.sin_addr.s_addr = INADDR_ANY;
    server_address_info.sin_port = htons(PORT);

    if (bind(server_socket_file_descriptor, (struct sockaddr*)&server_address_info, sizeof(server_address_info)) < 0) {
        printf("bind 실패\n");
        exit(1);
    }

    if (listen(server_socket_file_descriptor, 1) < 0) {
        printf("listen 실패\n");
        exit(1);
    }
    printf("서버 시작, 포트 %d에서 대기 중...\n", PORT);

    client_socket_file_descriptor = accept(server_socket_file_descriptor, (struct sockaddr*)&client_address_info, &client_address_length);
    if (client_socket_file_descriptor < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(client_address_info.sin_addr));

    // 데이터 수신 후 삭제
    int bytes_received;
    while ((bytes_received = recv(client_socket_file_descriptor, buff, BUFSIZE, 0)) > 0) {
        printf("수신: %d bytes\n", bytes_received);
        memset(buff, 0, BUFSIZE);
    }

    printf("클라이언트 연결 종료\n");

    close(client_socket_file_descriptor);
    close(server_socket_file_descriptor);
    return 0;
}