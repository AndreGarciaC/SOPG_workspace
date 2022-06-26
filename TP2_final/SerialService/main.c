#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "SerialManager.h"
#include <strings.h>

#define BUFFER_SIZE 9
#define S_MNGR_PORT 1
#define BAUDRATE 115200

char buffer_s[BUFFER_SIZE];
socklen_t addr_len;
struct sockaddr_in clientaddr;
struct sockaddr_in serveraddr;
char buffer[128];
int newfd;
int n;
int s;
bool flg_tcp_connected = false;

/**
 * @brief Rutina de apertura de socket 
 * 
 */
static void start_socket()
{
	s = socket(AF_INET,SOCK_STREAM, 0);

	bzero((char*) &serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(10000);
	if(inet_pton(AF_INET, "127.0.0.1", &(serveraddr.sin_addr))<=0)
	{
		fprintf(stderr,"ERROR invalid server IP\r\n");
	}

	if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
		close(s);
		perror("listener: bind");
	}

	if (listen (s, 10) == -1) // backlog=10
	{
		perror("error en listen");
	}
	addr_len = sizeof(struct sockaddr_in);
}

/**
 * @brief Rutina de apertura de puerto serial
 * 
 */
static void start_serial()
{
	if (serial_open(S_MNGR_PORT, BAUDRATE) != 0)
	{
		printf("Error abriendo puerto serie\n");
		exit(1);
	}
	else
	{
		printf("Puerto abierto\n");
	}
}

/**
 * @brief Rutina de bloqueo de señales.
 * 
 */
void block_sign(void)
{
	sigset_t set;
	int s;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	//sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_BLOCK, &set, NULL);
}

/**
 * @brief Rutina de desbloqueo de señales.
 * 
 */
void unblock_sign(void)
{
	sigset_t set;
	int s;
	sigemptyset(&set);
	sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	//sigaddset(&set, SIGUSR1);
	pthread_sigmask(SIG_UNBLOCK, &set, NULL);
}

/**
 * @brief Hilo de comunicación desde el emulador hacia la interfaz.
 * 
 * @param message Texto a mostrar cuando inicia el presente hilo.
 * @return void* NULL
 */
void* hilo_1 (void* message)
{
	unblock_sign();
	while(1)
	{
		printf ("%s\n", (const char *) message);
		int bytes = serial_receive(buffer_s,BUFFER_SIZE);
		if(bytes>0)
		{
			while(1)
			{
				newfd = accept(s, (struct sockaddr *)&clientaddr,&addr_len);
				if ( newfd== -1)
				{
					perror("error en accept");
					exit(1);
				}
				printf("server: conectado:\n");

				if (write (newfd, buffer_s, 5) == -1)
				{
					perror("Error escribiendo mensaje en socket");
					exit (1);
				}
				usleep(1000);
			}
		}
		usleep(1000);
	}

	return NULL;
}

/**
 * @brief Hilo de comunicación desde la interfaz hacia el emulador.
 * 
 * @param message Texto a mostrar cuando inicia el presente hilo. 
 * @return void* NULL
 */
void* hilo_2 (void* message)
{
	unblock_sign();
	while(1)
	{
		printf ("%s\n", (const char *) message);
		int bytes = serial_receive(buffer_s,BUFFER_SIZE);
		if(bytes>0)
		{
			while(1)
			{

				newfd = accept(s, (struct sockaddr *)&clientaddr,&addr_len);
				if ( newfd== -1)
				{
					perror("error en accept");
					exit(1);
				}
				printf("server: conectado:\n");
				flg_tcp_connected = true;
				while (flg_tcp_connected)
				{
					if( (n = read(newfd,buffer_s,BUFFER_SIZE)) == -1 )
					{
						perror("Error leyendo mensaje en socket");
						exit(1);
					}


					buffer[n]=0x00;
					printf("Recibi %d bytes.:%s\n",n,buffer);

					if(newfd<0)
					{
						flg_tcp_connected = false;
					}
					usleep(1000);
				}
				usleep(1000);
			}
		}
		usleep(1000);
	}

	return NULL;
}

/**
 * @brief Rutina de handler de las senales. 
 * 
 * @param sig 
 */
void sigint_handler(int sig)
{
	kill(getpid(),SIGTERM);
}

int main (void)
{
	start_socket();
	start_serial();

	block_sign();

	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	pthread_t thread1, thread2;
	const char *message1 = "Hilo 1";
	const char *message2 = "Hilo 2";

	pthread_create (&thread1, NULL, hilo_1, (void *) message1);
	pthread_create (&thread2, NULL, hilo_2, (void *) message2);

	pthread_join (thread1, NULL);
	pthread_join (thread2, NULL);

	exit(EXIT_SUCCESS);
	return 0;
}
