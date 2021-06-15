#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include "common.h"
#define MAXCLI 15
#define BUF_SIZE 4096  //MAX BUFFER SIZE


void echo_server(int sockfd) {
    char buff[MSG_LEN];
    
    while (1) {
        // Cleaning memory
        memset(buff, 0, MSG_LEN);
        // Receiving message
        if (recv(sockfd, buff, MSG_LEN, 0) <= 0) {
            break;
        }
        printf("Received: %s", buff);
        // Sending message (ECHO)
        if (send(sockfd, buff, strlen(buff), 0) <= 0) {
            break;
        }
        printf("Message sent!\n");
    }
}


int allowclient (int fd_allowed){
    struct sockaddr_in client_addr;
    int client_socket = -1;
    memset(&client_addr, '\0', sizeof(client_addr));
    int socketlength = sizeof(struct sockaddr_in);
    if((client_socket = accept(fd_allowed, (struct sockaddr *)&client_addr, (socklen_t *)&socketlength)) == -1){
        perror("Error while accepting");
        return 1;
    }
    return client_socket;
}





int handle_bind(char * port) {
    struct addrinfo hints, *result, *rp;
    int sfd;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, port, &hints, &result) != 0) {
        perror("getaddrinfo()");
        exit(EXIT_FAILURE);
    }
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype,
        rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }
        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sfd);
    }
    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);
    return sfd;
}

int clientoutcome(int sockfd , char* buffer){
    memset(buffer, '\0', BUF_SIZE);
    int ter = read(sockfd ,buffer,BUF_SIZE);
    if(!strcmp(buffer, "/quit\n")){
            printf("exiting right away sir\n");
            close(sockfd);
        }
    if(ter != -1){
        write(sockfd, buffer, ter);
        printf("Received %s from %i\n", buffer, sockfd);
    }
    memset(buffer, '\0', BUF_SIZE);
    return ter;
}



int control_clients (int sockfd,char* buffer){
    int ndfs = MAXCLI;
    struct pollfd fds[ndfs];
    memset(fds,0,ndfs*sizeof(struct pollfd)); //Cleaning memory
    for (int i=0 ;i<ndfs;i++){
        fds[i].fd = -1;   //Iniatilize all events to -1
    }
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    while (1){
    int ret = poll(fds,MAXCLI,-1);
    if (ret == -1){
        perror("error while poll");
        return 1;
    }
    for (int i=0; i<MAXCLI;i++){
           if(i == 0 && (fds[i].revents & POLLIN) > 0){
               int clientfd = allowclient(sockfd);
               int current_fd =1;
               while(fds[current_fd].fd != -1 && current_fd < MAXCLI){
                    current_fd++;}
                    if(current_fd < MAXCLI){
                                fds[current_fd].fd = clientfd;
                                fds[current_fd].events = POLLIN;
                                printf("Assigned %i fd to client socket %i\n", current_fd, clientfd);
                    }else{
                                printf("Close one terminal down to continue\n");
                                close(clientfd);
                         }
                    fds[i].revents = 0;
           }else if (i!=0 &&(fds[i].revents & POLLHUP)){
               close(fds[i].fd);
               fds[i].fd = -1;
           }else if(i != 0 && (fds[i].revents & POLLIN)){
               clientoutcome(fds[i].fd, buffer);// fonction d'action d'un client!
           }
        }
    }
}



int main(int argc, char *argv[]) {
    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
    struct sockaddr cli;
    int sfd, connfd;
    socklen_t len;
    if( argc != 2 ){
        printf("Usage : pgm port#\n");
        return 0;
    }
    char * port = argv[1];//import port short number from TERMINAL
    sfd = handle_bind(port);
    if ((listen(sfd, SOMAXCONN)) != 0) {
        perror("listen()\n");
        exit(EXIT_FAILURE);
    }
    /*len = sizeof(cli);
    if ((connfd = accept(sfd, (struct sockaddr*) &cli, &len)) < 0) {
        perror("accept()\n");
        exit(EXIT_FAILURE);
    }
    echo_server(connfd);*/
    control_clients(sfd,buffer);

    close(sfd);
    return EXIT_SUCCESS;
}

