
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
//#include <Windows.h>
#include <pthread.h>
#include <malloc.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <netdb.h>


#include <sys/time.h>

#include <signal.h>




int sockfd;

//#define SERVER_IP	"106.13.62.194"
#define SERVER_IP	"127.0.0.1"

#define SERVER_PORT	6666

void printf_hex(char *buf, int len)
{
	int i;
	for(i = 0; i < len; i++)
	{
		printf("0x%x ", buf[i]);
	}

	
}


int main(void)
{
	char command[1024];
	char *str;
	/*	�����ߵ�������Ϣ */
	struct sockaddr_in their_addr;	

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		/*	���socket()���ó��ִ�������ʾ������Ϣ���˳� */
		perror("socket");
//		exit(1);
	}

	/*	�����ֽ�˳�� */
	their_addr.sin_family = AF_INET;
	/*	�����ֽ�˳�򣬶����� */
	their_addr.sin_port = htons(SERVER_PORT);
	their_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	/*	���ṹʣ�µĲ�������*/
	bzero(&(their_addr.sin_zero), 8);
	if(connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1)
	{
		/*	���connect()�������Ӵ�������ʾ��������Ϣ���˳� */
		perror("connect");
		exit(1);
	}

	

	ssize_t ret;
	char recvbuf[512];
	char *buf = "hello! I'm client!";
	
	while(1)
	{
		
		if((ret = send(sockfd, buf, strlen(buf) + 1, 0)) == -1)
		{
			perror("send : ");
		}

		sleep(2);
		if((ret = recv(sockfd, &recvbuf, sizeof(recvbuf), 0)) == -1){
			return -1;
		}

		printf("recv :\r\n");
		printf("%s", recvbuf);
		printf("\r\n");

		sleep(2);
	}
	

	close(sockfd);

	
	return 0;
}


