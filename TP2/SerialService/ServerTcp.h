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

int create_socket();
int server_process(int s,char * buffer_tcp[], int size);
int send_msg_client_tcp(int newfd,char msg[],int size);
int server_recv_msg(int newfd,char * buffer,int size);
