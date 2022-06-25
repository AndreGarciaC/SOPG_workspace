#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <pthread.h>

#include "SerialManager.h"
#include "SerialService.h"
#include "ServerTcp.h"

#define BUFFER_SIZE 9
#define S_MNGR_PORT 1
#define BAUDRATE 115200

char *buffer_s[];
char *buffer_tcp[];
int s_socket, s_serial, newfd;
int bytes_read;

static open_serial_port()
{
  if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
  {
    printf("Error abriendo puerto serie\n");
    exit(1);
  }
  else
  {
    printf("Puerto abierto\n");
    flg_sport_open = true;
  }
}
void send_frame_to_emulator(char *p_buffer)
{
  change_buffer_content(p_buffer, &buffer_tcp, BUFFER_SIZE);
  if (flg_sport_open)
  {
    serial_send(p_buffer, BUFFER_SIZE);
    memset(buffer_tcp, '\0', BUFFER_SIZE);
  }
}

void receive_frame_from_emulator(char *p_buffer)
{
  int bytes_read = serial_receive(p_buffer, BUFFER_SIZE);
  if (bytes_read > 0)
  {
    do
    {
      buffer_s[bytes_read] = '\0';
      bytes_read = serial_receive(p_buffer, BUFFER_SIZE);
    } while (bytes_read > 0);
    flg_srecv = true; 
  }
}

void *thread_serial_com(void *p_buffer_s)
{
  while (1)
  {
    printf("Hilo 1. Comunicación serial\n");
    // receive_frame_from_emulator(p_buffer_s);
    // if (flg_srecv && flg_socket_open)
    // {
    //   printf("Enviamos a Interfaz\n");
    //   if(accept_connection(s_socket, &newfd)>0)
    //   flg_connected = true;

    //   while(flg_connected)
    //   {
    //     if(send_msg_client_tcp(newfd, buffer_tcp, BUFFER_SIZE)==0)
    //       printf("enviado %s \n",buffer_tcp);
    //     else
    //       flg_connected = false;
    //   }
    // }
    sleep(1);
  }
  return NULL;
}

void *thread_tcp_com(void *p_buffer_tcp)
{
  while (1)
  {
    printf("Hilo 2. Comunicacion TCP\n");
    if(accept_connection(s_socket, &newfd)>0)
      flg_connected = true;
    while(flg_connected)
    {
      if(server_recv_msg(newfd, buffer_tcp, BUFFER_SIZE)==0)
      {
        memset(buffer_s,'\0',BUFFER_SIZE);
        strcpy(buffer_s,buffer_tcp);
        send_frame_to_emulator(buffer_s);
        printf("recibido %s \n",buffer_tcp);
      }
      else
        flg_connected = false;
    }
    sleep(1);
  }
  return NULL;
}

int main(void)
{
  
  s_socket = create_socket();
  if(s_socket>0)
  {
    flg_socket_open = true;
    printf("Socket creado\n");
  }
  else
  {
    printf("Error abriendo el socket\n");
    return -1;
  }
  
  open_serial_port();

  pthread_t serial_com, tcp_com;

  int ret_serial = pthread_create(&serial_com, NULL, thread_serial_com, (void *)buffer_s);
  if (!ret_serial)
  {
    errno = ret_serial;
    perror("pthread_create");
    return -1;
  }

  int ret_tcp = pthread_create(&tcp_com, NULL, thread_tcp_com, (void *)buffer_tcp);
  if (!ret_tcp)
  {
    errno = ret_tcp;
    perror("pthread_create");
    return -1;
  }
  pthread_join(serial_com, NULL);
  pthread_join(tcp_com, NULL);
  
  return 0;
}

  

