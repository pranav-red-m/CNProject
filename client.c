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

int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);

    int sockfd;
    char buffer[BUFFER_SIZE];
    char packet[BUFFER_SIZE];

    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    printf("Enter message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    buffer[strcspn(buffer,"\n")] = 0;

    unsigned int sum = checksum(buffer);

    sprintf(packet,"%s|%u",buffer,sum);

    sendto(sockfd, packet, strlen(packet), 0,
           (struct sockaddr*)&server_addr, sizeof(server_addr));

    memset(buffer,0,BUFFER_SIZE);

    recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);

    printf("Server reply: %s\n", buffer);

    closesocket(sockfd);
    WSACleanup();
}