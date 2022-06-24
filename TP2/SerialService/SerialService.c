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
int s_socket, s_serial;
static int bytes_read;
static void change_buffer_content(char *p_buffer_dst, char *p_buffer_src, int size)
{
  for (int i = 0; i < size; i++)
  {
    *p_buffer_dst++ = *p_buffer_src++;
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
    // memset(buffer_tcp, '\0', BUFFER_SIZE);
    // strcpy(buffer_tcp, buffer_s);
  }
}

void *thread_serial_com(void *p_buffer_s)
{
  printf("Hilo 1. Comunicación serial\n");
  // receive_frame_from_emulator(p_buffer_s);
  // if (flg_socket_recv)
  // {
  //   printf("Enviamos\n");
  //   send_frame_to_emulator(p_buffer_s);
  // }
}

void *thread_tcp_com(void *p_buffer_tcp)
{
  printf("Hilo 2. Comunicación TCP\n");
  server_process(s_socket, p_buffer_tcp, BUFFER_SIZE, &flg_srecv);
}

int main(void)
{

  s_socket = create_socket();
   server_process(s_socket, &buffer_tcp, BUFFER_SIZE, &flg_srecv);
  pthread_t serial_com, tcp_com;

  // if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
  // {
  //   printf("Error abriendo puerto serie");
  // }
  // else
  // {
  //   printf("Puerto abierto\n");
  //   flg_sport_open = true;
  // }
  
    // int ret_serial = pthread_create(&serial_com, NULL, thread_serial_com, (void *)buffer_s);
    // int ret_tcp = pthread_create(&tcp_com, NULL, thread_tcp_com, (void *)buffer_tcp);

    // if (!ret_serial)
    // {
    //   errno = ret_serial;
    //   perror("pthread_create");
    //   return -1;
    // }
    // if (!ret_tcp)
    // {
    //   errno = ret_tcp;
    //   perror("pthread_create");
    //   return -1;
    // }

    // pthread_join(serial_com, NULL);
    // pthread_join(tcp_com, NULL);

  return 0;
}
