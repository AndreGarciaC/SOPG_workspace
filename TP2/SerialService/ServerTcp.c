#include "ServerTcp.h"

#define SOCKET_PORT 10000
#define LOCAL_HOST "127.0.0.1"
#define SUBSTRING_1 ">SW"
#define SUBSTRING_2 ">OUT"

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
  return s;
}

int server_process(int s, char *buffer_tcp[], int size, bool *recv)
{
  while (1)
  {
    // Ejecutamos accept() para recibir conexiones entrantes
    printf("Ejecutamos accept\n");
    addr_len = sizeof(struct sockaddr_in);
    if (-1 == (newfd = accept(s, (struct sockaddr *)&clientaddr, &addr_len)))
    {
      perror("error en accept");
      exit(1);
    }

    char ipClient[32];
    inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
    printf("server:  conexion desde:  %s\n", ipClient);

    int error = 0;
    socklen_t len = sizeof(error);
    int retval = getsockopt(newfd, SOL_SOCKET, SO_ERROR, &error, &len);
    // Leemos mensaje de cliente
    if(server_recv_msg(newfd, buffer_tcp, size)==0)
      printf("recibido %s \n",buffer_tcp);
    //   if (strstr(buffer_tcp, SUBSTRING_2))
    //   {
    //     int server_recv_msg(newfd, buffer_tcp, size);
    //   }
    //   if (retval != 0)
    //     close(newfd);
    
    // if (strstr(buffer_tcp, SUBSTRING_1))
    // {
    printf("Comando recibido desde emulador\n");
    char msg[] = ">SW:1,0\r\n";
    send_msg_client_tcp(newfd, msg, size);
    //   *recv = false;
    // }
    // else{
    //   char msg[] = "prueba_envio";
    //   int send_msg_client_tcp(newfd, msg, size);
    // }
    }
    // Cerramos conexion con cliente

   // fin while
  printf("Sali recepcion \n");
}

int server_recv_msg(int newfd, char *buffer, int size)
{
  int bytes_read;
  char buffer_aux[size];
  if ((bytes_read = read(newfd, *buffer, size)) == -1)
  {
    // perror("Error leyendo mensaje en socket");
    // exit(1);
    return -1;
  }
  if (bytes_read > 0)
  {
    do
    {
      buffer[bytes_read] = '\0';
      bytes_read = read(newfd, *buffer, size);
    } while (bytes_read > 0);
    return 0;
}
}
int send_msg_client_tcp(int newfd, char msg[], int size)
{
  if (write(newfd, msg, size) == -1)
  {
    perror("Error escribiendo mensaje en socket");
    exit(1);
  }
}
