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
#include <stdbool.h>

int create_socket();
int accept_connection(int s, int * newfd);
int send_msg_client_tcp(int newfd, char *p_buffer, int size);
int server_recv_msg(int newfd,char * buffer,int size);
