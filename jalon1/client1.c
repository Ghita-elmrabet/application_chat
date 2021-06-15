#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "common.h"

void echo_client(int sockfd) {

	char buff[MSG_LEN];
	int n;
    int nfds =2;
    struct pollfd fds[nfds];
    printf("Message: \n");
    memset(fds,0,nfds*sizeof(struct pollfd));
    fds[0].fd = sockfd;
    fds[1].fd=0;
    fds[0].events = POLLIN;
    fds[1].events = POLLIN;
    fds[0].revents = 0;
    fds[0].revents = 0;

    while (1){

    int enabled = 0;
    enabled = poll(fds,nfds,-1);
    if ( 0>enabled){
        perror("error while poll");

    }

            if(fds[0].revents & POLLIN){
                fds[0].revents = 0;
                memset(buff, 0, MSG_LEN);
                if (recv(sockfd, buff, MSG_LEN, 0) <= 0) {
                    break;
                }
                printf("Received: %s", buff);
            }

        if(fds[1].revents & POLLIN){
            fds[1].revents = 0;
            // Cleaning memory
            memset(buff, 0, MSG_LEN);
            // Getting message from client

            n = 0;

            while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
            // Sending message (ECHO)
            if (send(sockfd, buff, strlen(buff), 0) <= 0) {
                break;
            }
            printf("Message sent!\n");
        }
    }
}
//Cette fonction permet de créer une socket et de se connecter au serveur en prenant comme argument l'addresse ip du serveur et le numéro de port
int handle_connect(char const *addr_ip , char const *port) {
	struct addrinfo hints, *result, *rp;
	int sfd;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if (getaddrinfo(addr_ip, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		sfd = socket(rp->ai_family, rp->ai_socktype,rp->ai_protocol);
		if (sfd == -1) {
			continue;
		}
		if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
			break;
		}
		close(sfd);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not connect\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return sfd;
}

int main(int argc, char const *argv[]){
	if( argc != 3 ){
    	printf("Usage : pgm ip_addr port#\n");
        return 0;
    }

    //  /*create socket + connection au serveur */
	int sfd = handle_connect(argv[1],argv[2]);
    // send message to server

    echo_client(sfd);
	close(sfd);
	return EXIT_SUCCESS;
}
