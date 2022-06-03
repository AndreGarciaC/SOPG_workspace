/**
 * @file writer.c
 * @author Andrea García (andregarciace@gmail.com)
 * @brief TP1: Proceso escritor de strings: entradas del usuario y señales.
 * @version 0.1
 * @date 2022-05-30
 *
 * @copyright Copyright (c) 2022
 *
 */
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>
#include <stdio.h>

#define FIFO_NAME "myfifo"
#define S1 "SIGN:1"
#define S2 "SIGN:2"
#define BUFFER_SIZE 300
char finalBuffer[BUFFER_SIZE];
int32_t fd;
char file_txt[7];

/**
 * @brief Handler de la señal SIGUSR1. Escribe su captura en el buffer.
 *
 * @param sig
 */
void signal_hdlr1(int sig)
{
  if (sig == SIGUSR1)
  {
    write(fd, S1, strlen(S1));
  }
}

/**
 * @brief Handler de la señal SIGUSR2. Escribe su captura en el name fifo.
 *
 * @param sig
 */
void signal_hdlr2(int sig)
{
  if (sig == SIGUSR2)
  {
    write(fd, S2, strlen(S2));
  }
}

int main(void)
{
  char bufferHeader[] = "DATA:";
  char outputBuffer[BUFFER_SIZE - sizeof(bufferHeader)];
  uint32_t bytesWrote;
  int32_t returnCode;

  // Creo name fifo
  if ((returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0)) < -1)
  {
    printf("Error creando named fifo: %d\n", returnCode);
    exit(1);
  }

  printf("Esperando proceso lector...\n");
  if ((fd = open(FIFO_NAME, O_WRONLY)) < 0)
  {
    printf("Error abriendo archivo fifo: %d\n", fd);
    exit(1);
  }

  printf("Proceso lector detectado... Tipea algo\n");

  while (1)
  {
    // Evalúo si captura las señales.
    if (signal(SIGUSR1, signal_hdlr1) == SIG_ERR)
      printf("\nNo se puede capturar señal SIGUSR1\n");
    if (signal(SIGUSR2, signal_hdlr2) == SIG_ERR)
      printf("\nNo se puede capturar señal SIGUSR2\n");

    // Almacena las entradas por consola en un buffer.
    fgets(outputBuffer, (sizeof(outputBuffer)), stdin);
    sprintf(finalBuffer, "%s%s", bufferHeader, outputBuffer);

    // Escribe el valor del buffer en name fifo.
    if ((bytesWrote = write(fd, finalBuffer, strlen(finalBuffer) - 1)) == -1)
    {
      perror("write");
    }
  }
  return 0;
}
