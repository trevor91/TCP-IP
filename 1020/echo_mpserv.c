#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //fork
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
	int serv_sock, clnt_sock;
	struct sockaddr_in serv_adr, clnt_adr;

	pid_t pid; //프로세스 번호를 저장하는 타입
	struct sigaction act;//void (*sa_handler)(int); sigset_t sa_mask; int sa_flags;
	socklen_t adr_sz;
	int str_len, state;
	char buf[BUF_SIZE];
	if(argc != 2)
	{
		printf("usage : %s <PORT>\n",argv[0]);
		exit(1);
	}
	//signal setting
	act.sa_handler = read_childproc;//signal 발생시 실행할 함수
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;

	state = sigaction(SIGCHLD, &act, 0); //(int signo, struct sigaction* act, struct sigaction* oldact) -> 시그널의 정보를 인자로 전달, 첫번째 인자로 전달된 상수에 해당하는 시그널이 발생시에 호출될 함수의 정보를 전달, 이전에 등록되었던 시늘 핸들러의 함수 포인터를 얻는데 사용되는 인자. 필요없으면 0을 전달.
//SIGCHLD : 자식 프로세스가 종료된 상황
	
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(atoi(argv[1]));

	if(bind (serv_sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
	{
		error_handling("bind() error");
	}
	if(listen(serv_sock, 5) == -1)
	{
		error_handling("listen() error");
	}

	while(1)
	{
		adr_sz = sizeof(clnt_adr);
		clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
		if(clnt_sock == -1)
		{
			continue;
		}
		else
		{
			puts("new client connected;");
		}
		pid = fork();
		if(pid == -1)
		{
			close(clnt_sock);
			continue;
		}
		if(pid == 0) // child processor
		{
			close(serv_sock);//자식 프로세서가 소켓도 복사해서..-> 동일한 포트에 할당된 소켓이 둘 이상이라서...
			while((str_len = read(clnt_sock, buf, BUF_SIZE)) != 0)
			{
				write(clnt_sock, buf, str_len);
			}
			close(clnt_sock);
			puts("client disconnected");
			return 0;
		}
		else //parent processor
		{
			close(clnt_sock); // 동일한 포트에 할당된 소켓이 둘 이상인것을 방지.
		}
	}
	close(serv_sock);
	return 0;
}
void read_childproc(int sig)
{
	pid_t pid;
	int status;
	pid = waitpid(-1, &status, WNOHANG); //좀비 프로세스의 생성을 막는 함수
	printf("removed proc id: %d \n", pid);
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}




