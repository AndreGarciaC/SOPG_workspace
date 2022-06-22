#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "SerialManager.h"
#include "SerialService.h"
#include "ServerTcp.h"

#define BUFFER_SIZE 9
#define S_MNGR_PORT 1
#define BAUDRATE 115200

static char buffer[BUFFER_SIZE];

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

int main(void)
{

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
  while (1)
  {
    if (create_server_socket() > 1)
    {
      printf("posible");
    }
  }
}
