#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

socklen_t addr_len;
struct sockaddr_in clientaddr;
struct sockaddr_in serveraddr;
char buffer[10];
int newfd;
int n,s;
int create_server_socket()
{
  s = socket(AF_INET, SOCK_STREAM, 0);

  if (s < 0)
  {
    printf("Unable to create socket\n");
    return -1;
  }
  // Cargamos datos de direccion de server
  bzero((char *)&serveraddr, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_port = htons(10000);
  serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  // Abrimos puerto con bind()
	if (bind(s, (struct sockaddr*)&serveraddr, sizeof(serveraddr)) == -1) {
		close(s);
		perror("listener: bind");
		return 1;
	}

	// Seteamos socket en modo Listening
	if (listen (s, 10) == -1) // backlog=10
  	{
    	    	perror("error en listen");
    		exit(1);
  	}

  while(1)
	{
		// Ejecutamos accept() para recibir conexiones entrantes
		addr_len = sizeof(struct sockaddr_in);
    		if ( (newfd = accept(s, (struct sockaddr *)&clientaddr,&addr_len)) == -1)
      		{
		      perror("error en accept");
		      exit(1);
	    	}

		char ipClient[32];
		inet_ntop(AF_INET, &(clientaddr.sin_addr), ipClient, sizeof(ipClient));
		printf  ("server:  conexion desde:  %s\n",ipClient);

		if( (n = read(newfd,buffer,128)) == -1 )
		{
			perror("Error leyendo mensaje en socket");
			exit(1);
		}
		buffer[n]=0x00;
		printf("Recibi %d bytes.:%s\n",n,buffer);


		// Enviamos mensaje a cliente
    		if (write (newfd, "chau", 5) == -1)
    		{
      			perror("Error escribiendo mensaje en socket");
      			exit (1);
    		}

}
}