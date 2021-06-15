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
#include "msg_struct.h"
#define BUF_SIZE 4096  //MAX BUFFER SIZE
#define clientsmax 10



//LISTE CHAINEE INFORMATIONS CLIENTS
struct clients_information{
    int fd;
    short port;
    struct in_addr adress_ip;
    //char * time;
    int status;
    char pseudo[BUF_SIZE];
    struct clients_information* next;
};

//VERIFYING IF PSEUDO ALREADY EXISTS
int already_exist(int sockfd,struct clients_information* INFO_CLIENTS,char* pseudo){
    while(INFO_CLIENTS != NULL){
        
        if (strcmp(INFO_CLIENTS->pseudo,pseudo)==0){
            return 1;
        }
            INFO_CLIENTS = INFO_CLIENTS->next;
         
    }
            return 0;
            }
//  //////////////
int echo_server(int sockfd,struct clients_information* INFO_CLIENTS) {
    struct message msgstruct;
    char buf[MSG_LEN];
    struct clients_information* COPY=INFO_CLIENTS;
    // Cleaning memory
    while(1){
    memset(&msgstruct, 0, sizeof(struct message));
    memset(buf, '\0', MSG_LEN);
    // Receiving structure
    int ret;
    if (recv(sockfd, &msgstruct, sizeof(struct message),0) <= 0) {
            break;
    }
     //Receiving message
    if ( -1 == (ret = read(sockfd, buf,msgstruct.pld_len))){
      perror("Error while reading");

    }
 printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
        printf("Received: %s", buf);
 //disconnect client
  if(strcmp(buf,"quit")==10){
    printf("client disconnected\n");
    close(sockfd);
      return 1;
  }
    
   
    // Send back data to client
        
       if(msgstruct.type ==NICKNAME_NEW)
       {
       
           
               char chaine[1024];
               if(already_exist(sockfd,COPY,msgstruct.infos)==1){
                   sprintf(chaine,"Sorry but this nickname already exists");
                   // Filling structure
                  sprintf(buf,"Error:try with another pseudo");
                      msgstruct.pld_len = strlen(chaine);
                      strncpy(msgstruct.nick_sender, "Toto", 4);
                      msgstruct.type = ECHO_SEND;
                      strncpy(msgstruct.infos, "\0", 1);
                   // Sending structure
                    if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                        perror("Error while reading3");
                     }
                   // Sending message
                    if (send(sockfd, chaine, msgstruct.pld_len, 0) <= 0) {
                         perror("Error while reading4");
                    }
               }
               else{
                   struct clients_information* COPY= INFO_CLIENTS;
                   while(COPY != NULL){
                       if(COPY->fd == sockfd){
                           strcpy(COPY->pseudo,msgstruct.infos);
                       }
                       COPY =COPY->next;
                   }
               
           
                    if(strcmp(msgstruct.nick_sender,"") == 0){
                   sprintf(chaine,"your pseudo has been modified successefully");
                    }
                    else {
                   sprintf(chaine,"Welcome dear %s  to the chat :\n",msgstruct.infos );
                        }
           
               // Filling structure
                 msgstruct.pld_len = strlen(chaine);
                 strncpy(msgstruct.nick_sender, "Toto", 4);
                 msgstruct.type = ECHO_SEND;
                 strncpy(msgstruct.infos, "\0", 1);
               // Sending structure
                if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                    perror("Error while reading1");
                 }
               // Sending message (ECHO)
                if (send(sockfd, chaine, strlen(chaine), 0) <= 0) {
                     perror("Error while reading2");
                }
               }
               
       }
        
        
        
        
        
       
    
	
   
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
        printf("binding....\n");
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
// Managing clients
void managing_clients(int server_socket){
    int ndfs = clientsmax;
    struct pollfd fds[ndfs];
    //Cleaning memory
    memset(fds,0,ndfs*sizeof(struct pollfd));
    fds[0].fd = server_socket;
    fds[0].events = POLLIN;
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
        
    }
        
    for (int i=0 ; i<ndfs ; i++){
        //first case
           if(i == 0 && (fds[i].revents & POLLIN)){
               fds[0].revents = 0;
               struct sockaddr_in client_addr;
               socklen_t size_addr =sizeof(struct sockaddr_in);
               int clientfd = -1;
               memset(&client_addr, '\0', sizeof(client_addr));
               
               if((clientfd = accept(server_socket, (struct sockaddr *)&client_addr, &size_addr)) == -1){
                   perror("Error while accepting");
                   
               }
            // we want to put this clientfd in the file descriptor table
            int fill;
            fill=fillin_pollfd_table(fds,ndfs,clientfd);
            
               struct clients_information* new_client=malloc(sizeof(struct clients_information));
               struct in_addr adress_ip=client_addr.sin_addr;
               short port =client_addr.sin_port;
               
               new_client->fd=clientfd;
               new_client->port=port;
               new_client->adress_ip=adress_ip;
               new_client->next=previous_information;
               new_client->status=1;
               //new-client->time;
               previous_information = new_client;
           }
        //case number 2:
           else if (i!=0 &&(fds[i].revents & POLLHUP)){
               close(fds[i].fd);
               fds[i].fd = -1;
               fds[i].revents = 0;
           }
        //case number 3
           else if (i!=0 && (fds[i].revents & POLLIN)){
             fds[i].revents=0;
                     // display
                  printf("socket server is %d\n", server_socket);
                  printf("client socket is %d\n", fds[i].fd);
            //receive and send back data to client
            echo_server(fds[i].fd,previous_information);
          
           }
        }
    }
    
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
    managing_clients(server_socket);
	     
    
	

	close(server_socket);
	return EXIT_SUCCESS;
}


