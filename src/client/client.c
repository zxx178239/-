/*************************************************************************
  > File Name: client.c
  > Author: zxx
  > Mail: zxx178239@163.com 
  > Created Time: Sun 01 Feb 2015 08:32:33 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define PATH_HEAD "/home/wd/wd/pipo"
#define SERVER_NAME "server.fifo"
int main(int argc, char *argv[])
{
	int ser_fifo;
	char fifo_name[128] = "";
	char path_name[128] = "";
	char send_path_name[128];
	char recv_path_name[128];
	int send_fifo, recv_fifo;
	char msg[1024];
	sprintf(path_name, "%s/%s", PATH_HEAD, SERVER_NAME);
	ser_fifo = open(path_name, O_WRONLY);
	if(ser_fifo == 0)
	{
		printf("open fail.\n");
		exit(1);
	}
	printf("connect serve success.   client id is %u \n", getpid());
	fd_set rd_set;
	FD_ZERO(&rd_set);
	memset(msg, 0, sizeof(msg));
	//create send_fifo

	memset(fifo_name, 0, sizeof(fifo_name));
	memset(path_name, 0, sizeof(path_name));
	sprintf(fifo_name, "%u_r", getpid());
	sprintf(path_name, "%s/%s", PATH_HEAD, fifo_name);
	if(mkfifo(path_name, 0666) == -1)
	{
		printf("_w fail\n");
		exit(1);
	}
	strcpy(recv_path_name, path_name);
	memset(fifo_name, 0, sizeof(fifo_name));
	memset(path_name, 0, sizeof(path_name));
	sprintf(fifo_name, "%u_w", getpid());
	sprintf(path_name, "%s/%s", PATH_HEAD, fifo_name);
	if(mkfifo(path_name, 0666) == -1)
	{
		printf("_r fail\n");
		exit(1);
	}
	strcpy(send_path_name, path_name);
	sprintf(msg, "%u on", getpid());
	write(ser_fifo, msg, strlen(msg));
	recv_fifo = open(recv_path_name, O_RDONLY);
	send_fifo = open(send_path_name, O_WRONLY);
	printf("ok\n");
	while(1)
	{
		FD_SET(0, &rd_set);
		FD_SET(recv_fifo, &rd_set);
		select(1024, &rd_set, NULL, NULL, NULL);
		if(FD_ISSET(0,&rd_set))
		{
			memset(msg, 0, sizeof(msg));
			if(read(0, msg, sizeof(msg)) == 0)
			{
				sprintf(msg, "%d off", getpid());
				write(ser_fifo, msg, strlen(msg));
				//close ser_fifo; close two fifo
				
				close(recv_fifo);
				close(send_fifo);
				unlink(send_path_name);
				unlink(recv_path_name);
				break;
			}else
			{
				write(send_fifo, msg, strlen(msg));
			}
		}

		if(FD_ISSET(recv_fifo, &rd_set))
		{
			memset(msg, 0, sizeof(msg));
			read(recv_fifo, msg, sizeof(msg));
			write(1, msg, strlen(msg));
		}
	}
}
