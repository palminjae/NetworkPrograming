#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>       // nanosleep() 사용하려면 필요

#define PORT 5375
#define SERVER_IP "127.0.0.1"  // 서버 IP (자기 자신 = localhost)
#define DURATION 30            // 몇 초 동안 전송할지