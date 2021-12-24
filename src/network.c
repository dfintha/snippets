// network.c
// Helper functions for POSIX network communication in C (sendall and recvall).
// Author: Dénes Fintha
// Year: 2021
// -------------------------------------------------------------------------- //

// Interface

#include <errno.h>
#include <stdint.h>
#include <sys/socket.h>

ssize_t sendall(int sockfd, const void *buffer, size_t length, int flags);
ssize_t recvall(int sockfd, void *buffer, size_t length, int flags);

// Implementation

ssize_t sendall(int sockfd, const void *buffer, size_t length, int flags) {
    size_t total = 0;
    size_t left = length;
    const uint8_t *bytes = (const uint8_t *) buffer;

    while (total < length) {
        int current = send(sockfd, bytes + total, left, flags);
        if (current == -1) {
            if (errno == EAGAIN)
                continue;
            return -1;
        }
        total += current;
        left -= current;
    }

    return total;
}

ssize_t recvall(int sockfd, void *buffer, size_t length, int flags) {
    size_t total = 0;
    size_t left = length;
    uint8_t *bytes = (uint8_t *) buffer;

    while (total < length) {
        int current = recv(sockfd, bytes + total, left, flags);
        if (current == -1) {
            if (errno == EAGAIN)
                continue;
            return -1;
        }
        if (current == 0)
            return total;
        total += current;
        left -= current;
    }

    return total;
}

// Demonstration

#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(void) {
    struct protoent *proto = getprotobyname("tcp");
    if (proto == NULL)
        return 1;

    int sockfd = socket(AF_INET, SOCK_STREAM, proto->p_proto);
    if (sockfd < 0)
        return 2;

    struct hostent *host = gethostbyname("neverssl.com");
    if (host == NULL)
        return 3;

    in_addr_t in_address = inet_addr(inet_ntoa(*(struct in_addr *) *(host->h_addr_list)));
    if (in_address == (in_addr_t) -1)
        return 4;

    struct sockaddr_in address;
    memset(&address, 0x00, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = in_address;
    address.sin_port = htons(80);

    if (connect(sockfd, (struct sockaddr *) &address, sizeof(struct sockaddr_in)) < 0)
        return 5;

    const char *request = "GET / HTTP/1.1\r\nHost: neverssl.com\r\n\r\n";
    if (sendall(sockfd, request, strlen(request), 0) < 0)
        return 6;

    char buffer[1024];
    memset(buffer, 0x00, 1024);
    if (recvall(sockfd, buffer, 1024, 0) < 1)
        return 7;

    printf("%s\n", buffer);
    close(sockfd);
    return 0;
}
