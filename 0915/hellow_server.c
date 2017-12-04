#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
void error_handling(char* message);

int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;
	socklen_t clnt_addr_size;

	char message[]="Hello World!";

	if(argc != 2)
	{
		printf("Usage : %s <port>\n", argv[0]);
		exit(1);
	}
	serv_sock = socket(PF_INET, SOCK_STREAM,0);
	//socket을 만드는데 PF_INET은 IPv4
	//SOCK_STREAM은  전송방식을 말하는데
	//SOCK_STREAM으로 전송한다는건 TCP를 사용한다는것과 일맥상통
	//3번째 인자자를 0으로 줘도 자동으로 TCP로 들어간다
	//생성이 안되면 -1을 반환
	
	if(serv_sock == -1)
	{
		error_handling("socket() error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr)); //clear struct
	//serv_addr이라는 구조체를 setting해준다
	serv_addr.sin_family = AF_INET; //AF_INET은위에서 사용한 PF_INET과 같은 상수를 같고있다.
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);//host add를 network 주소로 변경
	serv_addr.sin_port = htons(atoi(argv[1])); //host add To networ add

	//serv_sock과 위에서 setting한 serv_addr을 통해서
	//socket에 ip, port를 지정해준다.
	//=> socket통신에 사용할수 있도록 준비해준다
	if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) ==1 )
		error_handling("bind() error");

	if(listen(serv_sock, 5) == -1) // create 5 connection pool
		error_handling("listen() error");

	clnt_addr_size = sizeof(clnt_addr);
	clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	//accept는 클라이언트의 요청을 받아들이고 클라이언트와 통신하는 전용 소켓을 생성
	//serv_sock는 소켓 디스크립터
	//clnt_addr을 통해서 상대방 주소를 알아오고,
	//마지막 인자는 구조체의 크기
	if(clnt_sock == -1)
		error_handling("accrept() error");

	write(clnt_sock, message, sizeof(message)); //client에게 message를 보낸다. message사이즈만큼
	close(clnt_sock);
	close(serv_sock);

		
	return 0;
}

void error_handling(char* message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}

