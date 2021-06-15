//
//  serveur2.c
//  
//
//  Created by el mrabet ghita on 05/11/2020.
//

#include "serveur2.h"
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
#include <ctype.h>
#include "common.h"
#include "msg_struct.h"
#define BUF_SIZE 4096  //MAX BUFFER SIZE
#define clientsmax 10
#define false "false"
//LISTE CHAINEE INFORMATIONS CLIENTS
struct clients_information{
    int fd;
    short port;
    struct in_addr address_ip;
    char pseudo[120];
    int status ; // 1 is online , 0 is offline
    char * time;
    
    struct clients_information* next;
};

//VERIFYING IF THE PSEUDO ALREADY EXISTS
int already_exist(int sockfd,struct clients_information* INFO_CLIENTS,char* pseudo){
    while(INFO_CLIENTS != NULL){
        
        if (strcmp(INFO_CLIENTS->pseudo,pseudo)==0){
            return 1;
        }
            INFO_CLIENTS = INFO_CLIENTS->next;
         
    }
            return 0;
            }
int echo_server(int sockfd,struct clients_information* INFO_CLIENTS) {
    char buf[MSG_LEN];
    struct message msgstruct;
    memset(buf, '\0', MSG_LEN);
    memset(&msgstruct, 0, sizeof(struct message));
    struct clients_information* COPY=INFO_CLIENTS;
    int ret;
    if (recv(sockfd, &msgstruct, sizeof(struct message),0) <= 0) {
        perror("Error while reading");
    }
     //Receiving message
    if ( -1 == (ret = read(sockfd, buf,msgstruct.pld_len))){
      perror("Error while reading");

    }
    
    printf("[YOUR CLIENT SENT YOU THIS,let's give him an answer ]: %s\n", buf);
  
   
    // Send  data to client
    char chaine[MSG_LEN];
    char copy_buff[MSG_LEN];
    char copy2[MSG_LEN];
    char copy3[MSG_LEN];
    memset(copy2, '\0',MSG_LEN);

    
    //disconnect client
    
     if(strcmp(buf,"/quit")==10){
         char disconnected[MSG_LEN];
         memset(disconnected, '\0', MSG_LEN);
         COPY=INFO_CLIENTS;
         while (COPY->next!= NULL)
         {
            if( COPY->fd==sockfd && COPY->status ==1){
                        COPY->status=0;
                printf("client :%s disconnected\n",COPY->pseudo);
             }
             COPY = COPY->next;
          }
         
         //filling the structure
         strcpy(disconnected,"You are disconnected");
         msgstruct.pld_len = strlen(buf);
         strncpy(msgstruct.nick_sender, "Toto", 4);
         msgstruct.type = ECHO_SEND;
         strncpy(msgstruct.infos,disconnected,strlen(disconnected));
       // Sending structure
                   if (write(sockfd, &msgstruct, sizeof(msgstruct)) <= 0) {
            perror("Error while reading111");
         }
       // Sending message (ECHO)
           
        if (write(sockfd, disconnected, strlen(disconnected)) <= 0) {
             perror("Error while reading222");
        }
         close(sockfd);
         
       }
       
         
     
    
    // if the user wants to get a new pseudo or change the old one
    if (msgstruct.type == NICKNAME_NEW)
    {
        
        
        if(already_exist(sockfd,COPY,msgstruct.infos)==1){
            sprintf(chaine,"Sorry but this nickname already exists\n");
            
            // Filling structure
           
               msgstruct.pld_len = strlen(chaine);
               strncpy(msgstruct.nick_sender, "Toto", 4);
               msgstruct.type = ECHO_SEND;
               strncpy(msgstruct.infos, "\0", 1);
            // Sending structure
             if (write(sockfd, &msgstruct, sizeof(msgstruct)) <= 0) {
                 perror("Error while reading3");
              }
            // Sending message
             if (write(sockfd, chaine, msgstruct.pld_len) <= 0) {
                  perror("Error while reading4");
             }
            return 1;
        }
        else{
            struct clients_information* COPY= INFO_CLIENTS;
            while(COPY->next != NULL){
                if(COPY->fd == sockfd ){
                    strcpy(COPY->pseudo,msgstruct.infos);
                }
                COPY =COPY->next;
            }
        
            int i = strcmp(msgstruct.nick_sender,"");
          
           
            if(strcmp(msgstruct.nick_sender,"")==0){
                // first we have to check that the pseudo chosen has no special characters
                char *name = msgstruct.infos;
               
                int a =9;
                int counter =0;
           
                for (int i ; i<strlen(msgstruct.infos)-1;i++)
                    {
                        
                    if (msgstruct.infos[i]!='\0' && isalpha(msgstruct.infos[i])==0){
                        if((isdigit(msgstruct.infos[i])==0)){
                            
                            
                            a=1;
                            break;
                        }
                    }
                           
                   
                }
                           
                if (a==1){
                    sprintf(chaine,"sorry your pseudo is invalid.\n");
                }
                // the pseudo has been accepted
                else {
                    sprintf(chaine,"Welcome on the chat dear %s \n",msgstruct.infos );
                }
                a=0;
            }
    
            else {
                sprintf(chaine,"the pseudo has been modified successefully\n");
                 }
            
        // Filling structure
          msgstruct.pld_len = strlen(chaine);
          strncpy(msgstruct.nick_sender, "Toto", 4);
          msgstruct.type = ECHO_SEND;
          strncpy(msgstruct.infos, "\0", 1);
        // Sending structure
           
         if (write(sockfd, &msgstruct, sizeof(msgstruct)) <= 0) {
             perror("Error while reading1");
          }
        // Sending message (ECHO)
            
         if (write(sockfd, chaine, strlen(chaine)) <= 0) {
              perror("Error while reading2");
         }
        }
    }
   // if the user wants to send a message to a particular user(he should be online so we always verify that his status is 1)
    else if ( msgstruct.type ==UNICAST_SEND){
        COPY = INFO_CLIENTS;
        while(COPY->next != NULL){
            
            if(strcmp(COPY->pseudo,msgstruct.infos)==10 && COPY->status==1){
                strncpy(msgstruct.nick_sender, "Toto", 4);
                    msgstruct.pld_len = strlen(buf);
                     msgstruct.type = ECHO_SEND;
                    strncpy(msgstruct.infos, "\0", 1);
                         if (send(COPY->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                perror("Error while reading");
                      }
                    // Sending message (ECHO)
                    if (send(COPY->fd, buf, msgstruct.pld_len, 0) <= 0) {
                         perror("Error while reading");
                   }
                 
                
             }
            COPY = COPY->next;
    }
    }
    // if the user wants to know the online users.
    else if ( msgstruct.type ==NICKNAME_LIST){
        COPY = INFO_CLIENTS;
        memset(chaine, '\0',MSG_LEN);
      
        char online_users[MSG_LEN];
        strncpy(online_users, "-Users online are :\n",strlen("-Users online are :\n"));
        
        while(COPY->next != NULL ){
            if(COPY->status ==1){
                char user[MSG_LEN];
            memset( user,'\0',MSG_LEN);
                sprintf(user,"\t - %s",COPY->pseudo);
                strcat(online_users,user);
            }
            
                
            
                       
                 
                
            
            COPY = COPY->next;
            
            
        }
        //filling structure
        strncpy(msgstruct.nick_sender, "Toto", 4);
            msgstruct.pld_len = strlen(online_users);
             msgstruct.type = ECHO_SEND;
            strncpy(msgstruct.infos, "\0", 1);
        if (send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
               perror("Error while reading ");
     }
   // Sending message (ECHO)
   if (send(sockfd, online_users, msgstruct.pld_len, 0) <= 0) {
        perror("Error while reading ");
  }
        memset( online_users,'\0',MSG_LEN);
    }
    // if the user wants to send a message to all the other users
    else if ( msgstruct.type ==BROADCAST_SEND){
        COPY = INFO_CLIENTS;
        while(COPY->next != NULL){
            if(  COPY->fd != sockfd ){
               
            
                strncpy(msgstruct.nick_sender, "Toto", 4);
                    msgstruct.pld_len = strlen(buf);
                     msgstruct.type = ECHO_SEND;
                    strncpy(msgstruct.infos, "\0", 1);
                         if (send(COPY->fd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
                                perror("Error while reading BROAD");
                      }
                    // Sending message (ECHO)
                    if (send(COPY->fd, buf, msgstruct.pld_len, 0) <= 0) {
                         perror("Error while reading BROAD2");
                   }
                 
                
             }
            COPY = COPY->next;
        }
            

    }
         // if the user wants to get details about the an online user
    else if ( msgstruct.type ==NICKNAME_INFOS){
        
            int exist;
            exist=1;
            COPY =INFO_CLIENTS;
            while (COPY->next != NULL){
                
                
                if (strcmp(COPY->pseudo,msgstruct.infos)==0 && COPY->status==1){
                   
                    char *ip_adress =inet_ntoa(COPY->address_ip);
                    
                    sprintf(copy2,"%s connected since %s with IP address %s and port number %i\n",COPY->pseudo,COPY->time,ip_adress,COPY->port);
                   
                    exist = 0;
                }
                COPY = COPY->next;
                
            }
            // Managing errors
            if (exist == 1) {
                strcpy(copy2,"the client you are looking for does not exist");
                
            }
        // Filling structure
          msgstruct.pld_len = strlen(copy2);
          strncpy(msgstruct.nick_sender, "Toto", 4);
          msgstruct.type = ECHO_SEND;
          strncpy(msgstruct.infos, "\0", 1);
        // Sending structure
         if (write(sockfd, &msgstruct, sizeof(msgstruct)) <= 0) {
             perror("Error while reading1");
          }
        // Sending message (ECHO)
         if (write(sockfd, copy2, strlen(copy2)) <= 0) {
              perror("Error while reading2");
         }
    }
    
    
    // if the user sends to the server a message that the server can not deal with
    else{
        char error[MSG_LEN];
        memset( error,'\0',MSG_LEN);
        COPY=INFO_CLIENTS;
        strncpy(error,"sorry you are disconnected because your message does not respect the structure ordered",strlen("sorry you are disconnected because your message does not respect the structure ordered"));
        while(COPY->next !=NULL)
        {
           if( COPY->fd==sockfd && COPY->status ==1){
                       COPY->status=0;
              
            }
            COPY = COPY->next;
         }
        // Filling structure
          msgstruct.pld_len = strlen(error);
          strncpy(msgstruct.nick_sender, "Toto", 4);
          msgstruct.type = ECHO_SEND;
          strncpy(msgstruct.infos, "\0", 1);
     if(send(sockfd, &msgstruct, sizeof(msgstruct), 0) <= 0) {
         perror("Error while reading12");
        }
        // Sending message (ECHO)
        if (send(sockfd, error, msgstruct.pld_len, 0) <= 0) {
            perror("Error while reading14");
        }
        close(sockfd);
        }
      
    
    return 1;
    
   
    
}
// CREATE SOCKET SERVER AND BIND TO THE SERVER
int handle_bind(char * port) {
    struct addrinfo hints, *result, *rp;
    int server_socket;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
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
        close(server_socket);
    }
    
    if (rp == NULL) {
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }
    printf("binding........ \n");
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
int managing_clients(int server_socket ){
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
               printf("A NEW CLIENT IS ACCEPTED , i'm a happy functional server. \n");
            // we want to put this clientfd in the file descriptor table
            int fill;
            fill=fillin_pollfd_table(fds,ndfs,clientfd);
            fds[0].revents = 0;
               struct clients_information* new_client=malloc(sizeof(struct clients_information));
               struct in_addr adress_ip=client_addr.sin_addr;
               printf("ip address is:%s\n ",inet_ntoa(adress_ip));
               short port =client_addr.sin_port;
               printf("port is :%i\n",port);
               printf("ip address is:%s\n ",inet_ntoa(adress_ip));
               //defining local time
              
               time_t seconds = time(NULL);
               struct tm * timeinfo = localtime(&seconds);
               char temps [1024];
               sprintf(temps,"%04d-%02d-%02d %02d:%02d:%02d\n",1900+timeinfo->tm_year,timeinfo->tm_mon+1,timeinfo->tm_mday,timeinfo->tm_hour,timeinfo->tm_min,timeinfo->tm_sec);
                   
               new_client->time=temps;
               new_client->fd=clientfd;
               new_client->port=port;
               new_client->address_ip=adress_ip;
               new_client->time=temps;
               new_client->status=1;
               strcpy(new_client->pseudo,"naamee");
               
               
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
               
               
               int echo = echo_server(fds[i].fd,previous_information);
           }
        }
    }
    
    
    
    

    close(server_socket);
    return 1;
}
int main(int argc, char *argv[]) {
  
    char buffer[MSG_LEN];
    memset(buffer, '\0', MSG_LEN);
    struct sockaddr client;
    int server_socket, connexionfd;
    socklen_t len;
    if( argc != 2 ){
        printf("Usage : pgm port#\n");
        return 0;
    }
    char * port = argv[1];
    // bind
    server_socket = handle_bind(port);
    //listen
    if ((listen(server_socket, SOMAXCONN)) != 0) {
        perror("listen()\n");
        exit(EXIT_FAILURE);
    }
    printf("Listenning..........\n");
    
    
    
    
    
    

    int manage = managing_clients(server_socket);
    return EXIT_SUCCESS;
}


