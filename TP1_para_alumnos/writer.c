#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <signal.h>

#define FIFO_NAME "myfifo"
#define BUFFER_SIZE 300

char finalBuffer[BUFFER_SIZE];
void signal_hdlr1(int sig)
{
  if (sig == SIGUSR1)
  {
    strcpy(finalBuffer,"SIGN:1");
    printf("Signal 1 %s\n", finalBuffer);
  }
}

void signal_hdlr2(int sig)
{
  if (sig == SIGUSR2)
  {
    strcpy(finalBuffer,"SIGN:2");
  }
}
int main(void)
{
  char bufferHeader[]="DATA:";
  char outputBuffer[BUFFER_SIZE-sizeof(bufferHeader)];

  uint32_t bytesWrote;
  int32_t returnCode, fd;

  /* Create named fifo. -1 means already exists so no action if already exists */
  if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1 )
  {
    printf("Error creating named fifo: %d\n", returnCode);
    exit(1);
  }

  /* Open named fifo. Blocks until other process opens it */
  printf("waiting for readers...\n");
  if ( (fd = open(FIFO_NAME, O_WRONLY) ) < 0 )
  {
    printf("Error opening named fifo file: %d\n", fd);
    exit(1);
  }

  /* open syscalls returned without error -> other process attached to named fifo */
  printf("got a reader--type some stuff\n");

  struct sigaction sa;

	sa.sa_handler = &signal_hdlr1;
	sa.sa_flags = SA_RESTART;
	sigaction(SIGUSR1,&sa,NULL);

  signal(SIGUSR2, signal_hdlr2);
  /* Loop forever */
  while (1)
  {
    fgets(outputBuffer,(sizeof(outputBuffer)), stdin);
    sprintf(finalBuffer, "%s%s", bufferHeader,outputBuffer);
    /* Get some text from console */
    /* Write buffer to named fifo. Strlen - 1 to avoid sending \n char */
    if(strcmp(finalBuffer, "SIGN:1") == 0)
    {
      write(fd, finalBuffer, strlen(finalBuffer)-1);
    }
    if ((bytesWrote = write(fd, finalBuffer, strlen(finalBuffer)-1)) == -1)
    {
      perror("write");
    }
    else
    {
      printf("writer: wrote %d bytes\n", bytesWrote);
    }


  }
  return 0;
}
