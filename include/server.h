#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>
#define PATH_HEAD "/home/wd/wd/pipo"
#define SERVER_NAME "server.fifo"
typedef struct ClientNode
{
	int send_fifo;		//open's first num
	int recv_fifo;
	int process_id;
	struct ClientNode *m_next;
}CLIENTNODE, *pCLIENTNODE;

void dis_list_sendMsg(pCLIENTNODE head, int cur_id, char *msg);
