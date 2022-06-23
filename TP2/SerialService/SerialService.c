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

char * buffer_s[];
char * buffer_tcp[];

static void change_buffer_content(char *p_buffer, char *p_msg, int size)
{
  for (int i = 0; i < size; i++)
  {
    *p_buffer++ = *p_msg++;
  }
}

void send_frame_to_emulator(char *p_buffer, char *p_msg)
{
  change_buffer_content(p_buffer, p_msg, strlen(p_msg));
  if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
  {
    printf("Error abriendo puerto serie");
  }
  else
    serial_send(p_buffer, strlen(p_buffer));
}

void receive_frame_from_emulator(char *p_buffer, char *p_msg)
{
  if (serial_receive(p_buffer, BUFFER_SIZE) > 0)
  {
    flg_srecv = true;
  }
}

void receive_frame_from_interface(int s, char *p_buffer)
{
}

void *thread_from_interface(void *p_buffer)
{
  printf("Hilo 2");
  sleep(1);
  return NULL;
}

void *thread_from_emulator(void *p_buffer)
{
  int s;

  if (serial_receive(buffer_s, BUFFER_SIZE) > 0)
  {
    flg_srecv = true;
    memset(buffer_tcp, '\0', BUFFER_SIZE);
    strcpy(buffer_tcp,buffer_s);
  }

  s = create_socket();
  server_process(s,buffer_tcp,BUFFER_SIZE);

  return NULL;
}
int main(void)
{
  char s_buffer[BUFFER_SIZE];
  pthread_t data_from_interface, data_from_emulator;

  if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
  {
    printf("Error abriendo puerto serie");
  }

  pthread_create(&data_from_interface, NULL, thread_from_interface, (void *)buffer);
  pthread_create(&data_from_emulator, NULL, thread_from_emulator, (void *)buffer);

  pthread_join(data_from_interface, NULL);
  pthread_join(data_from_emulator, NULL);
  return 0;
  // char message[] = ">OUT:1,0\r\n";
  // if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
  // {
  //   flg_sport_open = true;
  //   printf("Error abriendo puerto serie");
  // }
  // else
  // {
  //   // send_frame_to_emulator(buffer, message);
  //   while (1)
  //   {
  //     receive_frame_from_emulator(&buffer, message);
  //   }
  // }
}
