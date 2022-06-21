#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include "SerialManager.h"
#include "SerialService.h"

#define BUFFER_SIZE 100

void change_buffer_content(char * p_buffer, char * msg, int size)
{
  for(int i=0; i<size; i++)
  {
    *p_buffer++ = *msg++;
  }
}

int main(void)
{
  char buffer[BUFFER_SIZE];
  char message[]=">OUT:X,Y\r\n";
  change_buffer_content(buffer,message,strlen(message));
  printf("Intento: %s\n", buffer);

  if(serial_open(1,115200)!=0)
  {
    printf("Error abriendo puerto serie");
  }

  serial_send(buffer,BUFFER_SIZE);
  return 0;
}
