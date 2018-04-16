#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>

#define BUF_SIZE 1024
#define UDP_PORT 10000
#define TCP_PORT 9999
void error_handling(char *message);

int main(int argc, char* argv[])
{
	int udp_sock, tcp_sock;
	struct sockaddr_in tcp_serv_addr, udp_serv_addr, from_addr;
	char send_message[BUF_SIZE];
	char tcp_message[BUF_SIZE], udp_message[BUF_SIZE];
	int tcp_str_len, recv_len, recv_cnt;
	int udp_str_len;
	socklen_t adr_sz;

	if(argc!=2)
	{
		printf("Usage : %s <IP>\n", argv[0]);
		exit(1);
	}

	tcp_sock = socket(PF_INET, SOCK_STREAM, 0);
	if(tcp_sock == -1)
		error_handling("socket() error");
	
	udp_sock = socket(PF_INET, SOCK_DGRAM, 0);
	if(udp_sock == -1)
		error_handling("socket() error");

	memset(&tcp_serv_addr, 0, sizeof(tcp_serv_addr));
	memset(&udp_serv_addr, 0, sizeof(udp_serv_addr));

	tcp_serv_addr.sin_family = AF_INET;
	tcp_serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	tcp_serv_addr.sin_port = htons(TCP_PORT);

	udp_serv_addr.sin_family = AF_INET;
	udp_serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	udp_serv_addr.sin_port = htons(UDP_PORT);

	if(connect(tcp_sock, (struct sockaddr*)&tcp_serv_addr, sizeof(tcp_serv_addr))==-1)
		error_handling("connect() error!");

	while(1)
	{

		// send message to each of servers //

		fputs("Input message(q to quit) : ", stdout);
		fgets(send_message, BUF_SIZE, stdin);
		
		if(!strcmp(send_message, "quit\n") || !strcmp(send_message, "QUIT\n"))
			break;

		tcp_str_len = write(tcp_sock, send_message, strlen(send_message));
		sendto(udp_sock, send_message, strlen(send_message), 0, (struct sockaddr*)&udp_serv_addr,
				sizeof(udp_serv_addr));

		// receive messages from each servers //

		adr_sz = sizeof(from_addr);
		udp_str_len = recvfrom(udp_sock, udp_message, BUF_SIZE, 0,
				(struct sockaddr*)&from_addr, &adr_sz);

		recv_len = 0;
		while(recv_len<tcp_str_len)
		{
			recv_cnt=read(tcp_sock, &tcp_message[recv_len], BUF_SIZE-1);
			if(recv_cnt == -1)
				error_handling("read() error!");
			recv_len += recv_cnt;
		}
		tcp_message[recv_len] = 0;
		udp_message[udp_str_len] = 0;


		printf("message from tcp server : %s", tcp_message);
		printf("message form udp server : %s", udp_message);

		
	}

	close(tcp_sock);
	close(udp_sock);
	return 0;
}

void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n',  stderr);
	exit(1);
}









