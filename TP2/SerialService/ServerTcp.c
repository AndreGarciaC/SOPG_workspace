#include "ServerTcp.h"

#define SOCKET_PORT 10000
#define LOCAL_HOST "127.0.0.1"

socklen_t addr_len;
struct sockaddr_in clientaddr;
struct sockaddr_in serveraddr;
int newfd;
int n;

int create_socket()
{
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
  {
    printf("Unable to create socket\n");
    return -1;
  }
  return s;
}

int server_process(int s, char * buffer_tcp[], int size)
{
  // Cargamos datos de direccion de server
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(SOCKET_PORT);
  serveraddr.sin_addr.s_addr = inet_addr(LOCAL_HOST);

  // Abrimos puerto con bind()
  if (bind(s, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
  {
    close(s);
    perror("listener: bind");
    return 1;
  }

  // Seteamos socket en modo Listening
  if (listen(s, 10) == -1) // backlog=10
  {
    perror("error en listen");
    exit(1);
  }

  while (1)
  {
    // Ejecutamos accept() para recibir conexiones entrantes
    addr_len = sizeof(struct sockaddr_in);
    if ((newfd = accept(s, (struct sockaddr *)&clientaddr, &addr_len)) == -1)
    {
      perror("error en accept");
      exit(1);
    }

    char ipClient[32];
    inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
    printf("server:  conexion desde:  %s\n", ipClient);

    // Leemos mensaje de cliente
    int server_recv_msg(newfd, buffer_tcp, size);

    // Enviamos mensaje a cliente
    char msg[]="prueba_envio";
    int send_msg_client_tcp(newfd,msg,size);

    // Cerramos conexion con cliente
    close(newfd);
  } // fin while
}

int server_recv_msg(int newfd, char *buffer, int size)
{
  int n;
  char buffer_aux[size];
  if ((n = read(newfd, *buffer, size)) == -1)
  {
    perror("Error leyendo mensaje en socket");
    exit(1);
  }
  // for (int i = 0; i < size; i++)
  // {
  //    buffer_aux[i] = *buffer++;
  // }
  buffer[n] = 0x00;
  printf("Recibi %d bytes.:%s\n", n, buffer);
}

int send_msg_client_tcp(int newfd, char msg[], int size)
{
  if (write(newfd, msg, size) == -1)
  {
    perror("Error escribiendo mensaje en socket");
    exit(1);
  }
}
