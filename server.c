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
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    char buffer[BUFFER_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Server listening...\n");

    FILE *fp = NULL;
    int expected_seq = 0;

    while(1) {
        memset(buffer, 0, BUFFER_SIZE);

        recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                 (struct sockaddr*)&client_addr, &addr_len);

        // Handle START request
        if(strncmp(buffer, "START", 5) == 0) {
            printf("Client wants to start/resume transfer\n");

            fp = fopen("received_file.txt", "ab+");

            fseek(fp, 0, SEEK_END);
            long size = ftell(fp);

            expected_seq = size / CHUNK_SIZE;

            char reply[50];
            sprintf(reply, "RESUME|%d", expected_seq);

            sendto(sockfd, reply, strlen(reply), 0,
                   (struct sockaddr*)&client_addr, addr_len);

            continue;
        }

        int seq;
        unsigned int recv_checksum;
        char data[CHUNK_SIZE];

        sscanf(buffer, "%d|%[^|]|%u", &seq, data, &recv_checksum);

        unsigned int calc_checksum = checksum(data, strlen(data));

        if(seq == expected_seq && calc_checksum == recv_checksum) {
            fwrite(data, 1, strlen(data), fp);
            expected_seq++;
            printf("Packet %d OK\n", seq);
        } else {
            printf("Packet error or duplicate\n");
        }

        char ack[50];
        sprintf(ack, "ACK|%d", seq);

        sendto(sockfd, ack, strlen(ack), 0,
               (struct sockaddr*)&client_addr, addr_len);

        if(strcmp(data, "EOF") == 0)
            break;
    }

    if(fp) fclose(fp);
    closesocket(sockfd);
    WSACleanup();
    return 0;
}