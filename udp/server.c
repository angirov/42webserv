#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_CONN 5

int main() {
    int sockfd, clilen;
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

    // bind socket
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) { //https://stackoverflow.com/questions/21099041/why-do-we-cast-sockaddr-in-to-sockaddr-when-calling-bind
        perror("ERROR on binding");
        exit(1);
    }

    listen(sockfd, MAX_CONN); // N connection requests will be queued before further requests are refused.

    int bytes_read;
    char buf[512];
    do {
        bytes_read = recv(sockfd, buf, sizeof(buf)-1, 0);
        printf("Read %i bytes from FD %i\n", bytes_read, sockfd);

        if (bytes_read > 0) {
            buf[bytes_read] = '\0';
            printf(">> %s\n", buf);
        }
    } while (bytes_read >= 0);
}
