
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <netinet/in.h>
//#include <Windows.h>
#include <pthread.h>
#include <malloc.h>
#include <string.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <netdb.h>


#include <sys/time.h>

#include <signal.h>




#define SERVER_PORT_TCP			6666
#define TCP_BACKLOG 10


/* 在sock_fd 进行监听，在 new_fd 接收新的链接 */
int sock_fd, new_fd;


void printf_hex(char *buf, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		printf("0x%x ", buf[i]);
	}

	
}


void sig_chld(int signo)
{
	pid_t pid;
	int stat;

	while((pid = waitpid(-1, &stat, WNOHANG)) > 0)
		printf("child %d terminated\r\n", pid);
	return;
}





int main(void)
{
	char command[1024];
	char *str;

	/* 自己的地址信息 */
	struct sockaddr_in my_addr;
	/*	连接者的地址信息*/
	struct sockaddr_in their_addr;
	int sin_size;

	struct sockaddr_in *cli_addr;

	/* 1 、创建socket  */
	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket is error\r\n");
		exit(1);
	}

	/* 主机字节顺序 */
	/* 协议 */
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(6666);
	/* 当前IP 地址写入 */
	my_addr.sin_addr.s_addr = INADDR_ANY;

	/* 将结构体其余的都清零 */
	bzero(&(my_addr.sin_zero), 8);

	/* bind 绑定*/
	if(bind(sock_fd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
	{
		perror("bind is error\r\n");
		exit(1);
	}

	/* 开始监听 */
	if(listen(sock_fd, TCP_BACKLOG) == -1)
	{
		perror("listen is error\r\n");
		exit(1);
	}

	printf("start accept\n");

	/* 因为我们后面会创建出许多子进程来服务新的链接
		一旦子进程异常终止了，需要父进程来进行资源回收
	*/
	signal(SIGCHLD, sig_chld);			//在这里处理僵死进程

	/* accept() 循环 */
	while(1)
	{
		sin_size = sizeof(struct sockaddr_in);

		if((new_fd = accept(sock_fd, (struct sockaddr *)&their_addr, (socklen_t *)&sin_size)) == -1)
		{
			perror("accept");
			continue;
		}

		cli_addr = malloc(sizeof(struct sockaddr));

		printf("accept addr\r\n");

		if(cli_addr != NULL)
		{
			memcpy(cli_addr, &their_addr, sizeof(struct sockaddr));
		}

		//处理目标
		ssize_t ret;
		char recvbuf[512];
		char *buf = "hello! I'm server!";

		while(1)
		{
			if((ret = recv(new_fd, recvbuf, sizeof(recvbuf), 0)) == -1){
				printf("recv error \r\n");
				return -1;
			}
			printf("recv :\r\n");
			printf("%s", recvbuf);
			printf("\r\n");
			sleep(2);
			if((ret = send(new_fd, buf, strlen(buf) + 1, 0)) == -1)
			{
				perror("send : ");
			}

			sleep(2);
		}

		

		close(new_fd);
	}

	
	return 0;
}


