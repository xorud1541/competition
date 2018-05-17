#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
#define PORT 9999

void error_handling(char* message);
void *udp_server(void *arg);
void *tcp_server(void *arg);

int main(int argc, char* argv[])
{
	pthread_t t_id[2];
	int id, status;
	const char* tcp = "TCP ";
	const char* udp = "UDP ";

	id = pthread_create(&t_id[0], NULL, udp_server, (void*)udp);
	if(id<0){
		perror("udp thread create error()!");
		exit(0);
	}

	id = pthread_create(&t_id[1], NULL, tcp_server, (void*)tcp);
	if(id<0){
		perror("tcp thread create error()!");
		exit(0);
	}

	pthread_join(t_id[0], (void**)&status);
	pthread_join(t_id[1], (void**)&status);

	return 0;
}

void *udp_server(void *arg){
	char *name = (char*)arg;
	int serv_sock;
	char message[BUF_SIZE];
	char send_message[BUF_SIZE];
	int str_len;
	socklen_t clnt_adr_sz;

	struct sockaddr_in serv_adr, clnt_adr;

	serv_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(serv_sock == -1)
		error_handling("UDP socket error()!");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT);

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error!");

	while(1)
	{
		clnt_adr_sz = sizeof(clnt_adr);
		str_len = recvfrom(serv_sock, message, BUF_SIZE, 0,
				(struct sockaddr*)&clnt_adr, &clnt_adr_sz);
		printf("message from client : %s", message);
		strcpy(send_message, name);
		strcat(send_message, message);
		sendto(serv_sock, send_message, str_len+4, 0, (struct sockaddr*)&clnt_adr, clnt_adr_sz);
		memset(message, 0, BUF_SIZE);
		memset(send_message, 0, BUF_SIZE);
	}
	close(serv_sock);
}

void *tcp_server(void *arg){
	int serv_sock, clnt_sock;
	char message[BUF_SIZE];
	char send_message[BUF_SIZE];
	char *name = (char*)arg;
	int str_len;

	struct sockaddr_in serv_adr, clnt_adr;
	socklen_t clnt_adr_sz;

	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(serv_sock==-1)
		error_handling("socket() error!");

	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(PORT);

	if(bind(serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("bind() error!");
	if(listen(serv_sock, 5)==-1)
		error_handling("listen() error!");

	clnt_adr_sz = sizeof(clnt_adr);

	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &clnt_adr_sz);
	if(clnt_sock == -1)
		error_handling("accept() error!");

	while(1){
		while((str_len = read(clnt_sock, message, BUF_SIZE))!=0)
		{
			printf("message from client : %s", message);
			strcpy(send_message, name);
			strcat(send_message, message);
			write(clnt_sock, send_message, str_len+4);
			memset(send_message, 0x00, str_len+4);
			memset(message, 0x00, BUF_SIZE);
		}
	}
	close(clnt_sock);
	close(serv_sock);
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}



















