#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Checksum function
unsigned int checksum(char *data)
{
    unsigned int sum = 0;
    for(int i = 0; data[i] != '\0'; i++)
        sum += data[i];
    return sum;
}

int main() {

    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    int sockfd;
    char buffer[BUFFER_SIZE];

    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));

    printf("Server running on port %d...\n", PORT);

    while(1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                 (struct sockaddr*)&client_addr, &addr_len);

        char msg[BUFFER_SIZE];
        unsigned int received_checksum;

        sscanf(buffer, "%[^|]|%u", msg, &received_checksum);

        unsigned int calculated_checksum = checksum(msg);

        printf("\nClient %s:%d says: %s\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port),
               msg);

        if(calculated_checksum == received_checksum)
            printf("Checksum verified: Data integrity OK\n");
        else
            printf("Checksum mismatch! Data corrupted\n");

        char ack[] = "ACK from server";

        sendto(sockfd, ack, strlen(ack), 0,
               (struct sockaddr*)&client_addr, addr_len);
    }

    closesocket(sockfd);
    WSACleanup();
}