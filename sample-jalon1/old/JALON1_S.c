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
#define MAXCLI 10

struct info_clients {
	int fd;
  struct in_addr addr_ip;
  short port;
	struct info_clients* prev ;

};

void echo_server(int sockfd) {


	      // Receive data from client
	      char buf[MSG_LEN];
	      memset(buf, '\0', MSG_LEN);
	      int ret;
	      if ( -1 == (ret = read(sockfd, (void *)buf, MSG_LEN))){
	        perror("Error while reading");

	      }
        
       
        if(strcmp(buf,"quit")==10){
          printf("bbb\n");
          close(sockfd);
        } 
	      printf("%d bytes received, and buf is %s\n", ret, buf);
        
	      // Send back data to client
	      if( -1 == (ret = write(sockfd,
	        (void *)buf, strlen(buf))) ){
	        perror("Error while writing");

	      }
        
        
			

}

int handle_bind(char const *portt) {
	/* Create socket */
  printf("Creating socket...\n");
  int server_sock = socket(AF_INET, SOCK_STREAM, 0);

  // create server addr
  char  * addr_ip = "127.0.0.1";
  short port = atoi(portt);
  struct sockaddr_in  server_addr;
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family= AF_INET;
  server_addr.sin_port = htons(port);
  inet_aton(addr_ip,&(server_addr.sin_addr));

  // bind to server addr
  printf("Binding...\n");
  if( bind(server_sock,
    (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
    perror("Error while binding");

}
return server_sock;
}

int main(int argc, char const *argv[]) {
	struct sockaddr cli;
	int sfd;
	socklen_t len;
	sfd = handle_bind(argv[1]);
	if ((listen(sfd, MAXCLI)) != 0) {
		perror("listen()\n");
		exit(EXIT_FAILURE);
	}

	int ndfs = MAXCLI ;
  struct pollfd fds[ndfs];
  memset(fds, 0 ,ndfs*sizeof(struct pollfd));
  for (int i=0 ; i<ndfs ; i++){
       fds[i].fd=-1;

  }
  fds[0].fd=sfd;
  fds[0].events=POLLIN;

	struct info_clients* infos= malloc(sizeof(struct info_clients));

	infos->prev=NULL;

  while(1) {
    int ret=poll(fds,MAXCLI,-1);
    if (ret == -1){
      perror("error while poll");
      return 1;
    }
    for(int i=0; i< MAXCLI ; i++){
      if(i==0 && (fds[i].revents & POLLIN)){
        struct sockaddr_in client_addr;
        socklen_t size_addr = sizeof(struct sockaddr_in);
        printf("start accepting...\n");
        int client_fd = -1;
        if( -1 == ( client_fd = accept(sfd, (struct sockaddr *)&client_addr, &size_addr))){
          perror("Error while accepting");
          return 0;
        }
        int k=1;
        while (fds[k].fd != -1){
          k++;
        }
        fds[k].fd=client_fd;
        fds[k].events =POLLIN ;
        fds[0].revents=0;
			  struct info_clients* lastcli = malloc(sizeof(struct info_clients));

				lastcli->fd=client_fd;
       struct in_addr addr_ip =client_addr.sin_addr;
        lastcli->addr_ip=addr_ip;
        short portt=client_addr.sin_port;
        lastcli->port=portt;
				lastcli->prev=infos;
				infos=lastcli;

      }
      else if (i!=0 && (fds[i].revents & POLLIN)){
				// Affichage
			 printf("socket server is %d\n", sfd);
			 printf("client socket is %d\n", fds[i].fd);

      echo_server(fds[i].fd);

      }

		 }

 }

	close(sfd);
	return EXIT_SUCCESS;
}
