#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 30
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
	int sock;
	pid_t pid;
	char buf[BUF_SIZE];
	struct sockaddr_in serv_adr;
	
	struct timeval timeout; // setting timeout
	fd_set read, temp_read; // read file descripter set
	//fd_set write, temp_write; // write file descripter set
	int fd_max, str_len, fd_num, i;

	if(argc!=3) {
		printf("Usage : %s <IP> <port>\n", argv[0]);
		exit(1);
	}

	sock=socket(PF_INET, SOCK_STREAM, 0);  
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family=AF_INET;
	serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_adr.sin_port=htons(atoi(argv[2]));
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr))==-1)
		error_handling("connect() error!");

	//setting fd_set
	FD_ZERO(&read);
	//FD_ZERO(&write);
	FD_SET(0, &read);
	FD_SET(sock, &read);
//	FD_SET(sock, &write);
	fd_max = sock;

	while(1)
	{
		//temp_write = write;
		temp_read = read;
		
		//be like a book to timeout value
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		if((fd_num = select(fd_max+1, &temp_read, 0, 0, &timeout)) == -1)
		{
			break;
		}
		if(fd_num == 0) //timeout
			continue;

		//select return value 1~?.
		for(i=0;i<fd_max+1;i++)
		{
			//printf("%d\n", i);
			//read check.
			if(i==0)
			{
				//printf("is zero\n");
				if(FD_ISSET(i, &temp_read))
				{
					//printf("read\n");
					//read_routine(sock, buf);
					write_routine(sock, buf);
				}
			}
			else if(FD_ISSET(i,&temp_read)) //write check.
			{
				//printf("write\n");
				if(i == sock)
				{
					//printf("call read_routine()\n");
					//write_routine(sock, buf);
					read_routine(sock, buf);
				}
			}
		}
	}
	close(sock);
	return 0;
}

void read_routine(int sock, char *buf)
{
	int str_len=read(sock, buf, BUF_SIZE);
	if(str_len==0)
		return;

	buf[str_len]=0;
	printf("Message from server: %s", buf);
}
void write_routine(int sock, char *buf)
{
	fgets(buf, BUF_SIZE, stdin);
	if(!strcmp(buf,"q\n") || !strcmp(buf,"Q\n"))
	{	
		shutdown(sock, SHUT_WR);
		return;
	}
	write(sock, buf, strlen(buf));
}
void error_handling(char *message)
{
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}
