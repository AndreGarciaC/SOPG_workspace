int create_socket();
int server_open(int s, struct sockaddr_in serveraddr);
int server_connects(int s,int newfd,struct sockaddr_in clientaddr, socklen_t addr_len,char buffer[], int size);
int send_msg_client_tcp(int newfd,char msg[],int size);
int server_recv_msg(int newfd,char buffer[],int size);
