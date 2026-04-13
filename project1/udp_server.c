#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 4732
#define BUFSIZE 4096

int main() {
    int server_socket_file_descriptor;
    struct sockaddr_in server_address_info;
    struct sockaddr_in client_address_info;
    socklen_t client_address_length = sizeof(client_address_info);
    char buff[BUFSIZE];

    server_socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket_file_descriptor < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }
    // 서버 주소 설정
    memset(&server_address_info, 0, sizeof(server_address_info));
    server_address_info.sin_family = AF_INET;
    server_address_info.sin_addr.s_addr = INADDR_ANY;
    server_address_info.sin_port = htons(PORT);

    // bind (TCP랑 똑같아요)
    if (bind(server_socket_file_descriptor, (struct sockaddr*)&server_address_info, sizeof(server_address_info)) < 0) {
        printf("bind 실패\n");
        exit(1);
    }
    printf("UDP 서버 시작, 포트 %d에서 대기 중...\n", PORT);

    // 데이터 수신 (recvfrom = UDP 전용 recv)
    int bytes_received;
    while ((bytes_received = recvfrom(server_socket_file_descriptor, buff, BUFSIZE, 0,
                            (struct sockaddr*)&client_address_info, &client_address_length)) > 0) {
        printf("수신: %d bytes | 클라이언트: %s\n",
               bytes_received, inet_ntoa(client_address_info.sin_addr));
        memset(buff, 0, BUFSIZE);
    }

    printf("수신 종료\n");
    close(server_socket_file_descriptor);
    return 0;
}