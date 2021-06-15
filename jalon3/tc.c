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
#include "msg_struct.h"
#define MAXCLI 10
#define false
void echo_client(int sockfd) {

   struct message msgstruct;
   char buff[MSG_LEN];
   char  sender[MSG_LEN];
   memset(sender, 0, MSG_LEN);
   char salon1[MSG_LEN];
   memset(salon1, 0, MSG_LEN);
   char salon8[MSG_LEN];
   memset(salon8, 0, MSG_LEN);

 int n;
   int nfds =2;
   struct pollfd fds[nfds];
   printf("please login with /nick <your pseudo>: \n");
   memset(fds,0,nfds*sizeof(struct pollfd));
   fds[0].fd = sockfd;
   fds[1].fd=0;
   fds[0].events = POLLIN;
   fds[1].events = POLLIN;
   fds[0].revents = 0;
   fds[1].revents = 0;


   while (1){
   int enabled = 0;
   enabled = poll(fds,MAXCLI,-1);
   if ( 0>enabled){
       perror("error while poll");

   }
           if(fds[0].revents & POLLIN){
               fds[0].revents = 0;
               memset(buff, 0, MSG_LEN);
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
               strncpy(salon8,msgstruct.infos,strlen(msgstruct.infos));

               //printf("pld_len: %i / nick_sender: %s / type: %s / infos: %s\n", msgstruct.pld_len, msgstruct.nick_sender, msg_type_str[msgstruct.type], msgstruct.infos);
               printf("[YOU RECEIVED THIS]: %s\n", buff);
           }
       if(fds[1].revents & POLLIN){
           fds[1].revents = 0;
           // Cleaning memory
           memset(buff, 0, MSG_LEN);
           memset(&msgstruct, 0, sizeof(struct message));
           // Getting message from client
           n = 0;
           while ((buff[n++] = getchar()) != '\n') {} // trailing '\n' will be sent
           char copy_buff[MSG_LEN];
           strcpy(copy_buff,buff);

           // extracting the nickname
           char * pseudo;
           char groupe[MSG_LEN];
           memset(groupe, 0, MSG_LEN);
           char * pseudo1;
           size_t pld_Len=strlen(buff);


           const char * separators = " ";
           const char * msg_separator= "\n";

           char *command= strtok ( copy_buff, separators );
           //printf("%i\n",strcmp(command,"/channel_list\n"));// command contains the user's request for example /nick , /msgall ......
           // if the user wants to be disconnected
           if(strcmp(command,"/quit")==10){



             // Filling structure

               msgstruct.pld_len = pld_Len;
               strncpy(msgstruct.nick_sender,sender , strlen(sender));
               msgstruct.type = ECHO_SEND ;

               strncpy(msgstruct.infos, "\0",1);
               //sending structure
               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, command, msgstruct.pld_len, 0) <= 0) {
                   break;
               }
           }

           // if the user wants to get a new pseudo or change the old one
           else if(strcmp(command,"/nick")==0){
               // extracting the second word after /nick
               pseudo=strtok ( NULL, separators );
               //printf("commande %s\n",pseudo);
             // Filling structure

               msgstruct.pld_len = pld_Len;
               strncpy(msgstruct.nick_sender,sender , strlen(sender));
               msgstruct.type = NICKNAME_NEW ;
               strncpy(msgstruct.infos,pseudo,strlen(pseudo));
               strcpy(sender,pseudo);
               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, pseudo, msgstruct.pld_len, 0) <= 0) {
                   break;
               }
           }



           // if the user wants to get details about the an online user
           else if(strcmp(command,"/whois")==0){

               //filling the message structure
               pseudo1= strtok ( NULL, separators );

              // Filling structure
                msgstruct.pld_len = pld_Len;
                strncpy(msgstruct.nick_sender,sender , strlen(sender));
                msgstruct.type = NICKNAME_INFOS ;
                strncpy(msgstruct.infos,pseudo1,strlen(pseudo1));

               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, buff, msgstruct.pld_len, 0) <= 0) {
                   break;
               }

           }
           // if the user wants to know the online users.
           else if(strcmp(command,"/who")==10 || strcmp(command,"/who")==0 ){

               // Filling structure
                 msgstruct.pld_len = pld_Len;
                 strncpy(msgstruct.nick_sender,sender , strlen(sender));
                 msgstruct.type = NICKNAME_LIST ;
               strncpy(msgstruct.infos, "\0",1);
                 if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                        break;
                    }
                    // Sending message (ECHO)
                    if (send(sockfd, copy_buff, msgstruct.pld_len, 0) <= 0) {
                        break;
                    }


           }
           // if the user wants to send a message to a particular user(he should be online so we always verify that his status is 1)
           else if(strcmp(command,"/msg")==0){

               char  *receiver;
               char *copy_buff;

             receiver = strtok ( NULL, separators );

            strncpy(msgstruct.infos, receiver,strlen(receiver));
             copy_buff= strtok ( NULL, msg_separator); // to extract the message we want to send to the user we chose

             // Filling structure
               msgstruct.pld_len = pld_Len;
               strncpy(msgstruct.nick_sender,sender , strlen(sender));
               msgstruct.type = UNICAST_SEND ;
               if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                      break;
                  }
                  // Sending message (ECHO)
                  if (send(sockfd, copy_buff, msgstruct.pld_len, 0) <= 0) {
                      break;
                  }


           }

           // if the user wants to know the online users.
           else if(strcmp(command,"/msgall")==0){


               char *copy_buff1;

               copy_buff1= strtok ( NULL, msg_separator );

               strncpy(msgstruct.infos, "\0",1);

             // Filling structure
               msgstruct.pld_len = pld_Len;
               strncpy(msgstruct.nick_sender,sender , strlen(sender));
               msgstruct.type = BROADCAST_SEND ;
               if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                      break;
                  }
                  // Sending message (ECHO)
                  if (send(sockfd, copy_buff1, msgstruct.pld_len, 0) <= 0) {
                      break;
                  }
           }
           // if the user wants to create a group chat
           else if(strcmp(command,"/create")==0){

                               // extracting the second word after /nick
               strcpy(groupe,strtok ( NULL, separators ));


             // Filling structure

               msgstruct.pld_len = pld_Len;
               strncpy(msgstruct.nick_sender,sender , strlen(sender));
               msgstruct.type = MULTICAST_CREATE;
               strncpy(msgstruct.infos,groupe,strlen(groupe));

               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, groupe, msgstruct.pld_len, 0) <= 0) {
                   break;
               }
           }
           // if the user wants to get the list of the existing channels
           else if(strcmp(command,"/channel_list\n")==0   ){

               // Filling structure
                 msgstruct.pld_len = pld_Len;
                 strncpy(msgstruct.nick_sender,sender , strlen(sender));
                 msgstruct.type = MULTICAST_LIST;
               strncpy(msgstruct.infos, "\0",1);
                 if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                        break;
                    }
                    // Sending message (ECHO)
                    if (send(sockfd,msgstruct.infos , msgstruct.pld_len, 0) <= 0) {
                        break;
                    }


           }
           //if a user wants to quit a channel
           else if(strcmp(command,"/quit")==0){

               //filling the message structure
               strcpy(salon1,strtok ( NULL, separators ));

              // Filling structure
                msgstruct.pld_len = pld_Len;
                strncpy(msgstruct.nick_sender,sender , strlen(sender));
                msgstruct.type = MULTICAST_QUIT ;
                strncpy(msgstruct.infos,salon1,strlen(salon1));

               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, salon1, msgstruct.pld_len, 0) <= 0) {
                   break;
               }

           }
           else if(strcmp(command,"/join")==0){
               char salon4[MSG_LEN];
               memset(salon4, 0, MSG_LEN);
               //filling the message structure
               strcpy(salon4,strtok ( NULL, separators ));

              // Filling structure
                msgstruct.pld_len = pld_Len;
                strncpy(msgstruct.nick_sender,sender , strlen(sender));
                msgstruct.type = MULTICAST_JOIN;
                strncpy(msgstruct.infos,salon4,strlen(salon4));

               if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd, salon4, msgstruct.pld_len, 0) <= 0) {
                   break;
               }

           }


           else {

                //Filling structure
                 msgstruct.pld_len = strlen(buff);

                 strncpy(msgstruct.nick_sender,sender, strlen(sender));

                 msgstruct.type = MULTICAST_SEND;
                 strncpy(msgstruct.infos,salon8,strlen(salon8));
            if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                   break;
               }
               // Sending message (ECHO)
               if (send(sockfd,strtok ( copy_buff, separators ), strlen(strtok ( copy_buff, separators )), 0) <= 0) {
                   break;
               }

               }



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
