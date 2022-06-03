/**
 * @file reader.c
 * @author Andrea García (andregarciace@gmail.com)
 * @brief TP1: Proceso lector del buffer del proceso escritor. Clasifica los strings leídos en diferentes archivos de texto.
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
#include <stdio.h>

#define FIFO_NAME 	"myfifo"
#define S1 					"SIGN:1"
#define S2 					"SIGN:2"
#define WRITER_STR  "DATA:"
#define FILE1_NAME	"log"
#define FILE2_NAME	"signals"
#define BUFFER_SIZE 300

/**
 * @brief Abre, y en el caso de no existir, crea un archivo de texto en donde se adjunta un string al contenido.
 * 
 * @param file_name nombre del archivo de texto en donde se almacenará file_txt.
 * @param file_txt cadena de caracteres a almacenar en el archivo de texto.
 */
void write_line(char file_name[], char file_txt[])
{
	strcat(file_name, ".txt");
	FILE *fp;
	fp = fopen(file_name, "a");
	fprintf(fp, "%s\n", file_txt);
	fclose(fp);
}

int main(void)
{
	uint8_t inputBuffer[BUFFER_SIZE];
	int32_t bytesRead, returnCode, fd;
	char file_name[12];

	//Creo name fifo
	if ( (returnCode = mknod(FIFO_NAME, S_IFIFO | 0666, 0) ) < -1  )
	{
		printf("Error creating named fifo: %d\n", returnCode);
		exit(1);
	}

	printf("Esperando proceso escritor...\n");
	if ( (fd = open(FIFO_NAME, O_RDONLY) ) < 0 )
	{
		printf("Error abriendo archivo fifo: %d\n", fd);
		exit(1);
	}

	printf("Proceso escritor detectado\n");

	do
	{
		memset(file_name,'\0', 12);
		//Leo contenido de name fifo a través de su link descripitor y lo almaceno en un buffer.
		if ((bytesRead = read(fd, inputBuffer, BUFFER_SIZE)) == -1)
		{
			perror("read");
		}
		else
		{
			inputBuffer[bytesRead] = '\0';
			printf("lector: lee %d bytes: \"%s\"\n", bytesRead, inputBuffer);
			//Según el contenido del buffer, lo adjunto en su respectivo archivo de texto. 
			if (strstr(inputBuffer,S1) && !strstr(inputBuffer,WRITER_STR))
			{
				strncpy(file_name,FILE2_NAME,strlen(FILE2_NAME));
				write_line(file_name,inputBuffer);
			}
			else if (strstr(inputBuffer,S2) && !strstr(inputBuffer,WRITER_STR))
			{
				strncpy(file_name,FILE2_NAME,strlen(FILE2_NAME));
				write_line(file_name,inputBuffer);
			}
			else if (strstr(inputBuffer,WRITER_STR))
			{
				strncpy(file_name,FILE1_NAME,strlen(FILE1_NAME));
				write_line(file_name,inputBuffer);
			}

		}
	}
	while (bytesRead > 0);

	return 0;
}
