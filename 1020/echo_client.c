#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1024
void error_handling(char *message);

int main(int argc, char *argv[])
{
	int sock;
	char message[BUF_SIZE];
	int str_len;
	struct sockaddr_in serv_adr;

	if(argc!=3)
	{
		printf("Usage : %s <IP> <PORT>\n", argv[0]);
		exit(1);
	}
	sock = socket(PF_INET, SOCK_STREAM, 0); //(int domain, int type, int protocol)
	//PF_INET ipv4 internet protocol
	//SOCK_STREAM 연결 지향형 소켓 TCP <-> SOCK_DGRAM 비연결 지향형 소켓  UDP
	//IPPROTO_TCP, IPPROTO_UDP

	if(sock == -1)
	{
		error_handling("socket() error");
	}
	memset(&serv_adr, 0, sizeof(serv_adr)); //sockaddr_in val. clear
	serv_adr.sin_family = AF_INET; //AF_INET : IPv4 internet protocol
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]); //inet_addr() : 숫자와 점으로 이루어진 IP문자열 -> long헝의 숫자형태 IP로 변경
	serv_adr.sin_port = htons(atoi(argv[2])); // htons() : short integer -> network byte order

	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
	{
		error_handling("connect() error");
	}
	else
	{
		puts("connected.");
	}
	while(1)
	{
		fputs("input message(Q to quit): ", stdout); //(const char * str, FILE * stream) -> str이 가리키는 문자열을 stream에 쓴다
		fgets(message, BUF_SIZE, stdin); //(char *str, int num, FILE * stream) -> stream에서 문자열을 받는다(str은 읽은 문자열을 저장할 char arr pointer, num은 읽어들일 최대 문자 수)
		if(!strcmp(message, "q\n") || !strcmp(message,"Q\n"))
			break;
		write(sock, message, strlen(message)); //(int fd, void* buf, size_t cnt) -> 소켓 지정 번호, 보낼 데이터가 있는 버퍼, 보낼 데이터의 크기
		str_len = read(sock,message, BUF_SIZE-1); //(int fd, void* buf, size_t cnt) -> 소켓 지정 번호, 읽은 데이터가 저장될 공간, 읽은 데이터의 크기
		message[str_len] = 0;
		printf("message from server: %s", message);
	}
	close(sock);
	return 0;
}
void error_handling(char * message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
