#include "ServerTcp.h"

#define SOCKET_PORT 10000
#define LOCAL_HOST "127.0.0.1"

socklen_t addr_len;
struct sockaddr_in clientaddr;
struct sockaddr_in serveraddr;
int newfd;
int n;

void change_buffer_content(char *p_buffer_dst, char *p_buffer_src, int size)
{
  for (int i = 0; i < size; i++)
  {
    *p_buffer_dst++ = *p_buffer_src++;
  }
}
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
    return -1;
  }

  // Seteamos socket en modo Listening
  if (listen(s, 10) == -1) // backlog=10
  {
    perror("error en listen");
    exit(1);
  }
  return s;
}

int accept_connection(int s, int * newfd)
{
  printf("Ejecutamos accept\n");
  addr_len = sizeof(struct sockaddr_in);
  if (-1 == (*newfd = accept(s, (struct sockaddr *)&clientaddr, &addr_len)))
  {
    perror("error en accept");
    exit(1);
  }

  char ipClient[32];
  inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
  printf("server:  conexion desde:  %s\n", ipClient);

  int error = 0;
  socklen_t len = sizeof(error);
  int retval = getsockopt(*newfd, SOL_SOCKET, SO_ERROR, &error, &len);
  return retval;
}

int server_recv_msg(int newfd, char *p_buffer, int size)
{
  int bytes_read;
  if ((bytes_read = read(newfd, *p_buffer, size)) == -1)
  {
    perror("Error leyendo mensaje en socket");
    // exit(1);
    return -1;
  }
  if (bytes_read > 0)
  {
    do
    {
      p_buffer[bytes_read] = '\0';
      bytes_read = read(newfd, *p_buffer, size);
    } while (bytes_read > 0);
    memset(p_buffer, '\0', size);
    return 0;
  }
}

int send_msg_client_tcp(int newfd, char *p_buffer, int size)
{
  char buffer_aux[size];
  change_buffer_content(buffer_aux, p_buffer, size);

  if (write(newfd, buffer_aux, size) == -1)
  {

    perror("Error escribiendo mensaje en socket");
    exit(1);
  }
  memset(p_buffer, '\0', size);

  return 0;
}
