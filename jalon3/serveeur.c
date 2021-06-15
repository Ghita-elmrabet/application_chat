#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <time.h>

#include "common.h"
#include "msg_struct.h"
#define MAXCLI 10
struct info_clients {
  char  pseudo[MSG_LEN];
   int fd;
   char * time;
   int statut;
  struct in_addr addr_ip;
  short port;
	struct info_clients* prev ;

};
int pseudo_exist(int sockfd,char* pseudoo,struct info_clients* infos){
  
    while (infos != NULL)
          {
             if( strcmp(infos->pseudo,pseudoo)==0){
				         return 1;
                 
              }
              infos = infos->prev;
           }
     return 0;      
}
void echo_server(int sockfd,struct info_clients* infos) {
	      struct info_clients* infoss=infos;
          struct message msgstruct;
	      char buff[MSG_LEN];

	      // Cleaning memory
		memset(&msgstruct, 0, sizeof(struct message));
		memset(buff, 0, MSG_LEN);
		// Receiving structure
		if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
			perror("Error while reading");
		}
		// Receiving message
		if (recv(sockfd, buff, MSG_LEN, 0) <= 0){
			perror("Error while reading");
			
		}
		printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
		printf("Received: %s\n", buff);

     //close connection in the case of message s'/quit'
        if(strcmp(buff,"/quit")==10){
          struct info_clients* infoss=infos;
          while (infoss != NULL)
          {
             if( infoss->fd==sockfd){
				         infoss->statut=0;
                 
              }
              infoss = infoss->prev;
           }
          printf("connection closed ");
          close(sockfd);
          
        } 
    
		if(msgstruct.type == NICKNAME_NEW ){
      char buf[MSG_LEN];

        if(pseudo_exist(sockfd,msgstruct.infos,infoss)==1){
        
            // Filling structure
           sprintf(buf,"Error:try with another pseudo");
		       msgstruct.pld_len = strlen(buf);
		       strncpy(msgstruct.nick_sender, "Toto", 4);
		       msgstruct.type = ECHO_SEND;
	       	strncpy(msgstruct.infos, "\0", 1);
	      	// Sending structure
	       	if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
		       	perror("Error while reading");
	        	}
		       // Sending message (ECHO)
		        if (send(sockfd, buf, msgstruct.pld_len, 0) <= 0) {
		             perror("Error while reading");
		        }
		         printf("Message sent!\n");   

          }
      else{
           struct info_clients* infoss=infos;
          while (infoss != NULL)
          {
             if(infoss->fd == sockfd){
				      strcpy(infoss->pseudo,msgstruct.infos);
              }
             infoss = infoss->prev;
           }
           if(strcmp(msgstruct.nick_sender,"")!=0){

            
            sprintf(buf,"the pseudo has been modified");
          } 
          else{    
           sprintf(buf,"Welcome on the chat %s",msgstruct.infos);
            }

             // Filling structure
		       msgstruct.pld_len = strlen(buf);
		       strncpy(msgstruct.nick_sender, "Toto", 4);
		       msgstruct.type = ECHO_SEND;
	       	strncpy(msgstruct.infos, "\0", 1);
	      	// Sending structure
	       	if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
		       	perror("Error while reading");
	        	}
		       // Sending message (ECHO)
		        if (send(sockfd, buf, strlen(buf), 0) <= 0) {
		             perror("Error while reading");
		        }
		         printf("Message sent!\n");
       }
      
    }
    else if(msgstruct.type == NICKNAME_LIST){
      char buf[MSG_LEN];
      strcpy(buf,"Online users are:  \n      ");

      struct info_clients* infoss=infos;
          while (infoss != NULL)
          {
             if(infoss->statut==1){
				         strcat(buf,infoss->pseudo);
                
                 
              }
             infoss = infoss->prev;
           }
           
         // Filling structure
		       msgstruct.pld_len = strlen(buf);
		       
		       msgstruct.type = ECHO_SEND;
	       	strncpy(msgstruct.infos, "\0", 1);
	      	// Sending structure
	       	if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
		       	perror("Error while reading");
	        	}
		       // Sending message (ECHO)
		        if (send(sockfd, buf, strlen(buf), 0) <= 0) {
		             perror("Error while reading");
		        }
		         printf("Message sent!\n");

           


    }
    else if(msgstruct.type == UNICAST_SEND){

         struct info_clients* infoss=infos;
          while (infoss != NULL)
          {
             if(strcmp(infoss->pseudo,msgstruct.infos)==10){
               printf("hjhhhhhh");
				         msgstruct.pld_len = strlen(buff);
		              msgstruct.type = ECHO_SEND;
	       	          strncpy(msgstruct.infos, "\0", 1);
				          if (send(infoss->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
			                     perror("Error while reading");
		               }
	             	// Sending message (ECHO)
	             	if (send(infoss->fd, buff, msgstruct.pld_len, 0) <= 0) {
		              	perror("Error while reading");
	                }
                  
                 
              }
             infoss = infoss->prev;
           }  

       

    }
    else if(msgstruct.type == BROADCAST_SEND){
           char buf3[MSG_LEN];
           sprintf(buf3," [%s]:%s",msgstruct.nick_sender,buff);
           struct info_clients* infoss=infos;
          while (infoss != NULL)
          {
             if(infoss->statut == 1 && infoss->fd!=sockfd){
                  // Filling structure
		              msgstruct.pld_len = strlen(buf3);
		              strncpy(msgstruct.nick_sender, "Toto", 4);
		              msgstruct.type = ECHO_SEND;
	       	          strncpy(msgstruct.infos, "\0", 1);
				          if (send(infoss->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
			                     perror("Error while reading");
		               }
	             	// Sending message (ECHO)
	             	if (send(infoss->fd, buf3, msgstruct.pld_len, 0) <= 0) {
		              	perror("Error while reading");
	                }
              
              }
             infoss = infoss->prev;
           }




    }

    else if(msgstruct.type == NICKNAME_INFOS){
      char buf[MSG_LEN];
      struct info_clients* infoss=infos;
      int a=0;
          while (infoss != NULL)
          {
             if(strcmp(infoss->pseudo,msgstruct.infos)==0){
                 char *ip = inet_ntoa((infoss->addr_ip));
				         sprintf(buf,"%s connected since %s with IP address %s  and port number %i",infoss->pseudo,infoss->time,ip,infoss->port);
                 a=1;
                 
              }
             infoss = infoss->prev;
           }
           if(a==0){
             sprintf(buf,"this client doesn't exist");
           }
          if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
			perror("Error while reading");
		}
		// Sending message (ECHO)
		if (send(sockfd, buf, strlen(buf), 0) <= 0) {
			perror("Error while reading");
		}

   printf("Message sent!\n");   


    }
    else{
	      if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
			perror("Error while reading");
		}
		// Sending message (ECHO)
		if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
			perror("Error while reading");
		}
		 
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
  // listen
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
        fds[0].revents=0;
        struct sockaddr_in client_addr;
        socklen_t size_addr = sizeof(struct sockaddr_in);
        printf("start accepting...\n");
        int client_fd = -1;
        //accept
        if( -1 == ( client_fd = accept(sfd, (struct sockaddr *)&client_addr, &size_addr))){
          perror("Error while accepting");
          return 0;
        }
        // Affichage
             
			 printf("socket server is %d\n", sfd);
			 printf("client socket is %d\n", client_fd);

        int k=1;
        while (fds[k].fd != -1){
          k++;
        }
        //stock the data of new client in board 
        fds[k].fd=client_fd;
        fds[k].events =POLLIN ;
         
         
            
        //stock the data of new client in linked list
			  struct info_clients* lastcli = malloc(sizeof(struct info_clients));   
        lastcli->statut=1;  
        time_t timestamp = time( NULL );
         struct tm * timeInfos = localtime( & timestamp );
        lastcli->time= asctime( timeInfos ); 
				lastcli->fd=client_fd;
       struct in_addr addr_ip =client_addr.sin_addr;
        lastcli->addr_ip=addr_ip;
        short portt=client_addr.sin_port;
        lastcli->port=portt;
				lastcli->prev=infos;
				infos=lastcli;

      }
      else if (i!=0 && (fds[i].revents & POLLIN)){
        fds[i].revents=0;
				// display
			 printf("socket server is %d\n", sfd);
			 printf("client socket is %d\n", fds[i].fd);
       //receive and send back data to client
       echo_server(fds[i].fd,infos);


      }

		 }

 }
  // close connetion
	close(sfd);
  free(fds);
	return EXIT_SUCCESS;
}

