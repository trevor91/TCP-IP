#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#define BUF_SIZE 1024
void error_handling(char* message);

int main(int argc, char* argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr, clnt_adr;

	if(argc != 3)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM,0);
	//socket을 만드는데 PF_INET은 IPv4
	//SOCK_STREAM은  전송방식을 말하는데
	//SOCK_STREAM으로 전송한다는건 TCP를 사용한다는것과 일맥상통
	//3번째 인자자를 0으로 줘도 자동으로 TCP로 들어간다
	//생성이 안되면 -1을 반환
	
	if(sock == -1)
	{
		error_handling("socket() error");
	}

	memset(&serv_adr, 0, sizeof(serv_adr)); //clear struct
	//serv_addr이라는 구조체를 setting해준다
	serv_adr.sin_family = AF_INET; //AF_INET은위에서 사용한 PF_INET과 같은 상수를 같고있다.
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2])); //host add To networ add

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error");
	else
		puts("connected...");

	while(1)
	{
		fputs("input message(q to quit):", stdout);
		fgets(message, BUF_SIZE, stdin);

		if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
			break;

		write(sock, message ,strlen(message));
		str_len = read(sock, message, BUF_SIZE-1);
		message[str_len] = 0;
		printf("Message from server : %s ", message);
	}
	close(sock);
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

