// server.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

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
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    FILE *fp = fopen("received_file.txt", "wb");

    int expected_seq = 0;

    printf("Server listening...\n");

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);

        int recv_len = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                                (struct sockaddr*)&client_addr, &addr_len);

        int seq;
        unsigned int recv_checksum;
        char data[900];

        sscanf(buffer, "%d|%[^|]|%u", &seq, data, &recv_checksum);

        unsigned int calc_checksum = checksum(data, strlen(data));

        if(seq == expected_seq && calc_checksum == recv_checksum) {
            fwrite(data, 1, strlen(data), fp);
            printf("Received packet %d OK\n", seq);
            expected_seq++;
        } else {
            printf("Packet error or out of order\n");
        }

        // Send ACK
        char ack[50];
        sprintf(ack, "ACK|%d", seq);

        sendto(sockfd, ack, strlen(ack), 0,
               (struct sockaddr*)&client_addr, addr_len);

        // End condition (simple)
        if(strcmp(data, "EOF") == 0)
            break;
    }

    fclose(fp);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}