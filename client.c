// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024
#define CHUNK_SIZE 512

unsigned int checksum(char *data, int len) {
    unsigned int sum = 0;
    for(int i = 0; i < len; i++)
        sum += data[i];
    return sum;
}

int main() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    SOCKET sockfd;
    struct sockaddr_in server_addr;

    char buffer[BUFFER_SIZE];
    char packet[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    char ip[50];
    printf("Enter server IP: ");
    scanf("%s", ip);

    server_addr.sin_addr.s_addr = inet_addr(ip);

    FILE *fp = fopen("file.txt", "rb");

    int seq = 0;
    char data[CHUNK_SIZE];

    while(1) {
        int bytes = fread(data, 1, CHUNK_SIZE, fp);

        if(bytes <= 0) {
            strcpy(data, "EOF");
        }

        unsigned int sum = checksum(data, strlen(data));

        sprintf(packet, "%d|%s|%u", seq, data, sum);

        while(1) {
            sendto(sockfd, packet, strlen(packet), 0,
                   (struct sockaddr*)&server_addr, sizeof(server_addr));

            printf("Sent packet %d\n", seq);

            memset(buffer, 0, BUFFER_SIZE);

            struct timeval tv;
            tv.tv_sec = 2;
            tv.tv_usec = 0;

            setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                       (const char*)&tv, sizeof(tv));

            int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);

            if(recv_len > 0) {
                int ack_seq;
                sscanf(buffer, "ACK|%d", &ack_seq);

                if(ack_seq == seq) {
                    printf("ACK received for %d\n", seq);
                    break;
                }
            } else {
                printf("Timeout, resending packet %d\n", seq);
            }
        }

        seq++;

        if(strcmp(data, "EOF") == 0)
            break;
    }

    fclose(fp);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}