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
    int client_socket_file_descriptor;
    struct sockaddr_in server_address_info;
    char buff[BUFSIZE];

    memset(buff, 'A', BUFSIZE);  // 전송할 더미 데이터

    // 소켓 생성 (SOCK_DGRAM = UDP!)
    client_socket_file_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket_file_descriptor < 0) {
        printf("socket 생성 실패\n");
        exit(1);
    }

    // 서버 주소 설정
    memset(&server_address_info, 0, sizeof(server_address_info));
    server_address_info.sin_family = AF_INET;
    server_address_info.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_address_info.sin_port = htons(PORT);

    // 데이터 전송 (10번 반복)
    sendto(client_socket_file_descriptor, buff, BUFSIZE, 0,
       (struct sockaddr*)&server_address_info, sizeof(server_address_info));
    printf("전송 완료!\n");
    close(client_socket_file_descriptor);
    return 0;
}