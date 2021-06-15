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
#define MAXCLI 10
void echo_client(int sockfd) {
   // gestion of user and server messages with poll
  int ndfs = 2 ;
  struct pollfd fds[2];
  memset(fds, 0 ,ndfs*sizeof(struct pollfd));
  for (int i=0 ; i<ndfs ; i++){
       fds[i].fd=-1;
  }
  //initialisation
  fds[0].fd=STDIN_FILENO; 
  fds[0].events=POLLIN;
  fds[1].fd=sockfd ;    
  fds[1].events=POLLIN;
  char  nick_se[MSG_LEN];
  struct message msgstruct;
  char buff[MSG_LEN];
      int n;

      while (1) {
      int ret=poll(fds,MAXCLI,-1);
      if (ret == -1){
        perror("error while poll");
        break;
      }
       
      if((fds[0].revents & POLLIN)){
        fds[0].revents=0;
          // Cleaning memory
		memset(&msgstruct, 0, sizeof(struct message));
		memset(buff, 0, MSG_LEN);
		// Getting message from client
		
		n = 0;
		while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
    char buf2[MSG_LEN];
    strcpy(buf2,buff);
    size_t fullSize=strlen(buff);


		const char * separators = " ";
    const char * separatorss = "\n";
        char * strToken = strtok (buff,separators );
   if(strcmp(strToken,"/nick")==0){
			strToken = strtok ( NULL, separators );
             // Filling structure
		msgstruct.pld_len = fullSize;
		strncpy(msgstruct.nick_sender,nick_se , strlen(nick_se));
		msgstruct.type = NICKNAME_NEW ;
		strncpy(msgstruct.infos, strToken,strlen(strToken));  
	
     strcpy(nick_se,strToken);        
			  


		}
    else if(strcmp(buff,"/who")==0){
      
      // Filling structure
		msgstruct.pld_len = fullSize;
		strncpy(msgstruct.nick_sender,nick_se , strlen(nick_se));
		msgstruct.type = NICKNAME_LIST ;
		strncpy(msgstruct.infos, "\0",1);  
		         
    }
    else if(strcmp(strToken,"/msgall")==0){
      
      strToken = strtok ( NULL, separatorss );
     
      strcpy(buf2, strToken);
      // Filling structure
		msgstruct.pld_len = fullSize;
		strncpy(msgstruct.nick_sender,nick_se , strlen(nick_se));
		msgstruct.type = BROADCAST_SEND ;
		strncpy(msgstruct.infos, "\0",1);  
    

    }
    else if(strcmp(strToken,"/msg")==0){
      
      strToken = strtok ( NULL, separators );
     strcpy(msgstruct.infos, strToken);
     strToken = strtok ( NULL, separatorss );
      strcpy(buf2, strToken);
      // Filling structure
		msgstruct.pld_len = fullSize;
		strncpy(msgstruct.nick_sender,nick_se , strlen(nick_se));
		msgstruct.type = UNICAST_SEND ;
		
    

    }
    else if(strcmp(strToken,"/whois")==0){
       strToken = strtok ( NULL, separators );
        printf ("hhhhhh");
      // Filling structure
		msgstruct.pld_len = 0;
		strncpy(msgstruct.nick_sender,nick_se , strlen(nick_se));
		msgstruct.type = NICKNAME_INFOS ;
		strncpy(msgstruct.infos, strToken,strlen(strToken));  
		         
    }
		else{
		// Filling structure
		msgstruct.pld_len = fullSize;
		strncpy(msgstruct.nick_sender, nick_se , strlen(nick_se));
		msgstruct.type = ECHO_SEND;
		strncpy(msgstruct.infos, "\0", 1);
		
		printf("Message sent!\n");
		}
     
     // Sending structure
		if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
			break;
		}
		 //Sending message (ECHO)
		if (send(sockfd, buf2, strlen(buf2), 0) <= 0) {
			break;
		}
		printf("Message sent!\n");




    }


    if((fds[1].revents & POLLIN)){
      fds[1].revents=0;
          // Cleaning memory
		memset(&msgstruct, 0, sizeof(struct message));
		memset(buff, 0, MSG_LEN);
		// Receiving structure
		if (recv(sockfd, &msgstruct, sizeof(struct message), 0) <= 0) {
			break;
		}
		// Receiving message
		if (recv(sockfd, buff,MSG_LEN, 0) <= 0) {
			break;
		}
		printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
		printf("Received: %s\n", buff);
       
      }
  }



}

int handle_connect(char const * addr_ip,short port) {

    //Create socket 
  int fd = 0;
  fd =  socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
  printf("Connecting to server ... done!\n");
  printf("socket fd is %d\n", fd);
  printf(" [Server] : please login with /nick <your pseudo>\n ");



  // define the server address

  struct sockaddr_in  server_addr;
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family= AF_INET;
  server_addr.sin_port = htons(port);
  inet_aton(addr_ip,&(server_addr.sin_addr));

  //connection to serveur 
  if ( connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1 ){
    perror("Error connect");
    close(fd);
    exit(EXIT_FAILURE);
  }

  return fd;
}
int main(int argc, char const *argv[]) {

  char const  * addr_ip = argv[1];
  short port = atoi(argv[2]);
  int sfd;
  sfd = handle_connect(addr_ip,port);
  echo_client(sfd);
  close(sfd);
  return EXIT_SUCCESS;
}
