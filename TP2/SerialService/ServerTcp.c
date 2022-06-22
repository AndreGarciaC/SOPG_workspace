#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define SOCKET_PORT 10000
#define LOCAL_HOST  "127.0.0.1"

int create_socket()
{
  return socket(AF_INET, SOCK_STREAM, 0);
  if (s_aux < 0)
  {
    printf("Unable to create socket\n");
    return -1;
  }
}

int server_open(int s, struct sockaddr_in serveraddr)
{
  // Cargamos datos de direccion de server
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(SOCKET_PORT);
  serveraddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);

  // Abrimos puerto con bind()
  if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
    close(s);
    perror("listener: bind");
    return 1;
  }

  // Seteamos socket en modo Listening
  if (listen (s_aux, 10) == -1) // backlog=10
  {
    perror("error en listen");
    exit(1);
  }
}

int server_connects(int s,int newfd,struct sockaddr_in clientaddr, socklen_t addr_len,char buffer[], int size)
{
  int n;
  // Ejecutamos accept() para recibir conexiones entrantes
  addr_len = sizeof(struct sockaddr_in);
  if ( (newfd = accept(s, (struct sockaddr *)&clientaddr,&addr_len)) == -1)
  {
    perror("error en accept");
    exit(1);
  }

  char ipClient[32];
  inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
  printf  ("server:  conexion desde:  %s\n",ipClient);
}

int server_recv_msg(int newfd,char buffer[],int size)
{
  if( (n = read(newfd,buffer,size)) == -1 )
  {
    perror("Error leyendo mensaje en socket");
    exit(1);
  }
  buffer[n]=0x00;
  printf("Recibi %d bytes.:%s\n",n,buffer);
}
int send_msg_client_tcp(int newfd,char msg[],int size)
{
  if (write (newfd, msg, size) == -1)
  {
    perror("Error escribiendo mensaje en socket");
    exit (1);
  }
}
