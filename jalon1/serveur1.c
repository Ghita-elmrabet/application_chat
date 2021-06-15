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

#define BUF_SIZE 4096  //MAX BUFFER SIZE
#define clientsmax 10
//LISTE CHAINEE INFORMATIONS CLIENTS
struct clients_information{
    int fd;
    short port;
    struct in_addr adress_ip;
    struct clients_information* next;
};

// correspondance des adresses ipv4 et ipv6
/* char ipv4_TO_ipv6(char * addr_ipv4){


 }
 */
int echo_server(int sockfd) {
    char buf[BUF_SIZE];
    memset(buf, '\0', MSG_LEN);
    int ret;
    if ( -1 == (ret = read(sockfd, (void *)buf, BUF_SIZE))){
      perror("Error while reading");

    }

 //disconnect client
  if(strcmp(buf,"quit")==10){
    printf("client disconnected\n");
    close(sockfd);
      return 1;
  }
    printf("buf is %s\n", buf);

    // Send back data to client
    if( -1 == (ret = write(sockfd,
      (void *)buf, strlen(buf))) ){
      perror("Error while writing");

    }
    return 1;



}
// CREATE SOCKET SERVER AND BIND TO THE SERVER
int handle_bind(char * port) {
	struct addrinfo hints, *result, *rp;
	int server_socket;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &result) != 0) {
		perror("getaddrinfo()");
		exit(EXIT_FAILURE);
	}
	for (rp = result; rp != NULL; rp = rp->ai_next) {
		server_socket = socket(rp->ai_family, rp->ai_socktype,
		rp->ai_protocol);
		if (server_socket == -1) {
			continue;
		}
		if (bind(server_socket, rp->ai_addr, rp->ai_addrlen) == 0) {
			break;
		}
    printf("binding........ \n");
		close(server_socket);
	}
	if (rp == NULL) {
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	return server_socket;
}
int fillin_pollfd_table(struct pollfd table[],int size,int fd){
    int i=1;
    while (table[i].fd != -1){
      i++;
    }

    table[i].fd=fd;
    table[i].events=POLLIN;
    return 1;
}

int main(int argc, char *argv[]) {

    char buffer[BUF_SIZE];
    memset(buffer, '\0', BUF_SIZE);
	struct sockaddr client;
	int server_socket, connexionfd;
	socklen_t len;
	if( argc != 2 ){
    	printf("Usage : pgm port#\n");
        return 0;
    }
	char * port = argv[1];
	server_socket = handle_bind(port);
	if ((listen(server_socket, SOMAXCONN)) != 0) {
		perror("listen()\n");
		exit(EXIT_FAILURE);
	}
  printf("Listenning..........\n");

	// LA GESTION DES SOCKETS DES CLIENTS
    int ndfs = clientsmax;
    struct pollfd fds[ndfs];

    memset(fds,0,ndfs*sizeof(struct pollfd));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;//Cleaning memory
    // initializing the linked list of the clients information
    struct clients_information* previous_information=malloc(sizeof(struct clients_information));
    previous_information->next=NULL;
    for (int i=1 ;i<ndfs;i++){
        fds[i].fd = -1;
    }

    while (1){
    int enabled = 0;
    enabled = poll(fds,ndfs,-1);
    if ( 0>enabled){
        perror("error while poll");
        return 1;
    }

    for (int i=0 ; i<ndfs ; i++){
        //first case
           if(i == 0 && (fds[i].revents & POLLIN)){
               struct sockaddr_in client_addr;
               socklen_t size_addr =sizeof(struct sockaddr_in);
               int clientfd = -1;
               memset(&client_addr, '\0', sizeof(client_addr));

               if((clientfd = accept(server_socket, (struct sockaddr *)&client_addr, &size_addr)) == -1){
                   perror("Error while accepting");
                   return 0;
               }
            // when a client is connected we take his information
            int fill;
            fill=fillin_pollfd_table(fds,ndfs,clientfd);
            fds[0].revents = 0;
               struct clients_information* new_client=malloc(sizeof(struct clients_information));
               struct in_addr adress_ip=client_addr.sin_addr;
               short port =client_addr.sin_port;

               new_client->fd=clientfd;
               new_client->port=port;
               new_client->adress_ip=adress_ip;
               new_client->next=previous_information;
               previous_information = new_client;
           }
        //case number 2:
           else if (i!=0 &&(fds[i].revents & POLLHUP)){
               close(fds[i].fd);
               fds[i].fd = -1;
               fds[i].revents = 0;
           }
        //case number 3
           else if(i != 0 && (fds[i].revents & POLLIN)){

               printf("socket server is %d\n", server_socket);
               printf("client socket is %d\n", fds[i].fd);
               int echo = echo_server(fds[i].fd);
           }
        }
    }





	close(server_socket);
	return EXIT_SUCCESS;
}
