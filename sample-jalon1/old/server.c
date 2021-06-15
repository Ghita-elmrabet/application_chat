#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "common.h"

#define MAX 4096

int main(int argc, char const *argv[])
{

  /* Create socket */
  printf("Creating socket...\n");
  int server_sock = socket(AF_UNSPEC, SOCK_STREAM, 0);

  // create server addr
  char  * addr_ip = "127.0.0.1";
  short port = 8081;
  struct sockaddr_in  server_addr;
  memset(&server_addr, '\0', sizeof(server_addr));
  server_addr.sin_family= AF_UNSPEC;// famille d'adresses IPV4 et IPV4
  server_addr.sin_port = htons(port);//port
  inet_aton(addr_ip,&(server_addr.sin_addr));
    
  // bind to server addr
  printf("Binding...\n");
  if( bind(server_sock,
    (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1){
    perror("Error while binding");
    return 0;
  }

  // listen
  printf("Listening...\n");
  if (listen(server_sock, 10) == -1){
    perror("Error while listening");
    return 0;
  }

  while(1){
    // Accept incoming connection
    struct sockaddr_in client_addr;
    socklen_t size_addr = sizeof(struct sockaddr_in);
    printf("start accepting...\n");
    int client_fd = -1;
    if( -1 == ( client_fd = accept(server_sock, (struct sockaddr *)&client_addr, &size_addr))){
      perror("Error while accepting");
      return 0;
    }
    
    // Affichage
    printf("socket server is %d\n", server_sock);
    printf("client socket is %d\n", client_fd);

    // Receive data from client
    char buf[MAX];
    memset(buf, '\0', MAX);
    int ret;
    if ( -1 == (ret = read(client_fd, (void *)buf, MAX))){
      perror("Error while reading");
      return 0;
    }
    printf("%d bytes received, and buf is %s\n", ret, buf);

 

    // close socket fd
    close(client_fd);
  }
  return 0;


}

