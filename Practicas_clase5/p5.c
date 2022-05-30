//cliente
float num1,num2;
char op;
char bufferTx[128];

scanf("%f", &num1);
scanf("%c", &op);
scanf("%f", &num1);

//connect al server
sprintf(bufferTx, ">CMD:%c,%f,%f\n",op,num1,num2);

write(socket,bufferTx,strlen(bufferTx)+1);

//server
float num1,num2;
char op;
char bufferTx[128];

int n = read(socket,bufferRx,sizeof(bufferRx));
if(n>0)
{
	sscanf(bufferRx,">CMD:%c,%f,%f\n",&op,&num1,&num2)
	if(n=3)
	{
		switch(op)
		{
			case '+':res = num1+num2;break;
		}
	}
}

//MÉTODO BINARIO PARA EVITAR CONVERSIONES - puede que al trabjar
//con diferentes plataformas se desordenen los bits.
//cliente
struct S_request
{
	float num1,num2;
	char op;
	char bufferTx[128];
}
struct S_request req;
write(socket, &req, sizeof(req));

//server
struct S_request
{
	float num1,num2;
	char op;
	char bufferTx[128];
}
struct S_request req;
int n = read(socket, &req, sizeof(req));
