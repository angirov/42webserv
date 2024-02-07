#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_CONN 5

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // reset struct
    bzero((char *)&serv_addr, sizeof(serv_addr));
    unsigned portno = 5001;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); // convert values between host and network byte order

    char *hello = "hello from client";

    while(1) {
        sendto(sockfd, (const char *)hello, strlen(hello), 
        0, (const struct sockaddr*)&serv_addr, sizeof(serv_addr));
        printf("Wrote\n");
        sleep(3);
    }
}


// anoter way to test the server is with netcat: e.g. echo "Hello, UDP socket!" | nc -u -w1 localhost 5001