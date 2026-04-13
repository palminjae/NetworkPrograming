#include <stdio.h>
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <arpa/inet.h> // 소켓 주소, htons 네트워크 함수
#include <sys/socket.h> // socket,bind,listen,accept
#include <sys/time.h> // gettimeofday 시간 측정

#define PORT 5375
#define BUFSIZE 4096

int main() {
    int server_fd;
    struct sockaddr_in server_addr;
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    char buff[BUFSIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0 ); // af_inet -> IPv4 주소사용, sock_stream -> TCP사용, 0 -> 프로토콜 자동 선택
    if (server_fd <0) { // 실패시 음수 값 반환
        printf("socket 생성 실패");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr)); // 구조체 0으로 초기화
    server_addr.sin_family = AF_INET;             // IPv4 사용
    server_addr.sin_addr.s_addr = INADDR_ANY;     // 모든 IP에서 접속 허용
    server_addr.sin_port = htons(PORT);           // 포트 번호 설정

    // 소켓에 주소 연결
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("bind 실패");
        exit(1);
    }

    // 연결 대기
    if (listen(server_fd, 1) < 0) { // 인원수는 한명
        printf("listen 실패\n");
        exit(1);
    }
    printf("서버 시작, 포트 %d에서 대기 중...\n", PORT);

    // 클라이언트 수락
    client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        printf("accept 실패\n");
        exit(1);
    }
    printf("클라이언트 연결됨: %s\n", inet_ntoa(client_addr.sin_addr));

    // 데이터 수신 및 throughput 측정
    long long total_bytes = 0;
    int bytes_received;
    struct timeval start, now;
    gettimeofday(&start, NULL);  // 시작 시간 기록

    while ((bytes_received = recv(client_fd, buff, BUFSIZE, 0)) > 0) {
        total_bytes += bytes_received;

        // 현재 시간 측정
        gettimeofday(&now, NULL);
        double elapsed = (now.tv_sec - start.tv_sec) + 
                         (now.tv_usec - start.tv_usec) / 1e6;

        // throughput 출력
        if (elapsed > 0) {
            double throughput = (total_bytes * 8.0) / elapsed; // bps
            printf("수신: %lld bytes | 경과: %.2f초 | throughput: %.2f bps\n",
                   total_bytes, elapsed, throughput);
        }

        memset(buff, 0, BUFSIZE);  // 버퍼 초기화 (데이터 삭제)
    }

    printf("클라이언트 연결 종료\n");

    // 최종 결과 출력
    gettimeofday(&now, NULL);
    double total_elapsed = (now.tv_sec - start.tv_sec) + 
                           (now.tv_usec - start.tv_usec) / 1e6;
    printf("=== 최종 결과 ===\n");
    printf("총 수신 bytes: %lld\n", total_bytes);
    printf("총 시간: %.2f초\n", total_elapsed);
    if (total_elapsed > 0)
        printf("평균 throughput: %.2f bps\n", (total_bytes * 8.0) / total_elapsed);

    // 소켓 닫기
    close(client_fd);
    close(server_fd);
    return 0;
}