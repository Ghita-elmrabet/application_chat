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

/* Create socket */
 int fd = 0;
 fd =  socket(AF_UNSPEC,SOCK_STREAM,IPPROTO_TCP);
 printf("socket fd is %d\n", fd);
 
 /* definir de l'adresse du serveur */
 char  * addr_ip = "127.0.0.1";
 short port = 8081;
 struct sockaddr_in  server_addr;
 memset(&server_addr, '\0', sizeof(server_addr));
 server_addr.sin_family= AF_UNSPEC;
 server_addr.sin_port = htons(port);
 inet_aton(addr_ip,&(server_addr.sin_addr));

 /* connection au serveur */
 if ( connect(fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1 ){
   perror("Error connect");
   return 0;
 }

 // send message to server
 char buf[MAX];
 memset(buf, '\0', MAX);
 strcpy(buf, "Hello World!");
 int ret = 0;
 if ( (ret = write(fd, (void *)buf, strlen("Hello World!"))) == -1 ){
   perror("Error whiel writing");
   return 0;
 }
 printf("%d bytes have been sent\n", ret);

 // close connection
 sleep(5);
 close(fd);

 return 0;

}

