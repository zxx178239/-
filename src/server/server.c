/*************************************************************************
	> File Name: server.c
	> Author: zxx
	> Mail: zxx178239@163.com 
	> Created Time: Sun 01 Feb 2015 04:03:05 PM CST
 ************************************************************************/
#include "server.h"

int main(int argc, char *argv[])
{
	char path_name[128] = "";
	char fifo_name[128] = "";
	char msg[512] = "";
	char buf[1024] = "";
	char client_status[5];
	int client_id;
	sprintf(path_name, "%s/%s", PATH_HEAD, SERVER_NAME);
	int sev_fifo = open(path_name, O_RDONLY);	//readonly to open
	open(path_name, O_WRONLY);
	printf("ok\n");
	fd_set rd_set, bak_set;		//bak is rd's copy
	pCLIENTNODE phead, pnew, pcur, pre;
	phead = NULL;
	FD_ZERO(&rd_set);
	FD_ZERO(&bak_set);
	FD_SET(sev_fifo, &rd_set);

	while(1)
	{
		bak_set = rd_set;
		select(1024, &bak_set, NULL, NULL, NULL);
		printf("selecting...\n");
		if(FD_ISSET(sev_fifo, &bak_set))
		{
			//judge is on or off
			memset(msg, 0, sizeof(msg));
			if(read(sev_fifo, msg, sizeof(msg)) == 0)
			{
				printf("no client.\n");
				continue;
			}
			memset(client_status, 0, sizeof(client_status));
			sscanf(msg, "%d%s", &client_id, client_status);
			//on
			if(strcmp("on", client_status) == 0)
			{
				//open fifo; add listen; insert clientNode
				pnew = (pCLIENTNODE)calloc(1, sizeof(CLIENTNODE));
				if(pnew == NULL)
				{
					printf("pnew alloc fail.\n");
					exit(1);
				}

				memset(fifo_name, 0, sizeof(fifo_name));
				sprintf(fifo_name, "%d_r", client_id);
				memset(path_name, 0, sizeof(path_name));
				sprintf(path_name, "%s/%s", PATH_HEAD, fifo_name);
				pnew->process_id = client_id;
				pnew->send_fifo = open(path_name, O_WRONLY);

				memset(fifo_name, 0, sizeof(fifo_name));
				sprintf(fifo_name, "%d_w", client_id);
				memset(path_name, 0, sizeof(path_name));
				sprintf(path_name, "%s/%s", PATH_HEAD, fifo_name);
				pnew->process_id = client_id;
				pnew->recv_fifo = open(path_name, O_RDONLY);

				FD_SET(pnew->recv_fifo, &rd_set);

				//head_insert
				pnew->m_next = phead;
				phead = pnew;

				printf("client %d link success.\n", client_id);
			}else	//off
			{
				//close fifo; del listen; del clientNode
				pcur = phead;
				pre = NULL;
				while(pcur && pcur->process_id != client_id)
				{
					pre = pcur;
					pcur = pcur->m_next;
				}
				if(pcur == NULL)
				{
					printf("no exist.\n");
					continue;
				}else
				{
					if(pre == NULL)
					{
						phead = pcur->m_next;
					}else
					{
						pre->m_next = pcur->m_next;
					}
					close(pcur->send_fifo);
					close(pcur->recv_fifo);
					FD_CLR(pcur->recv_fifo, &rd_set);	
					free(pcur);
					printf("client %d close link.\n", client_id);
				}
			}
		}

		pcur = phead;
		while(pcur)
		{
			if(FD_ISSET(pcur->recv_fifo, &bak_set))
			{
				memset(msg, 0, sizeof(msg));
				read(pcur->recv_fifo, msg, sizeof(msg));
				memset(buf, 0, sizeof(buf));
				sprintf(buf, "from client %d: %s", pcur->recv_fifo, msg);
				dis_list_sendMsg(phead, pcur->recv_fifo, buf);
			}
			pcur = pcur->m_next;
		}
	}
}
