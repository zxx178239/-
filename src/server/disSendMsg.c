/*************************************************************************
	> File Name: disSendMsg.c
	> Author: zxx
	> Mail: zxx178239@163.com 
	> Created Time: Sun 01 Feb 2015 08:22:35 PM CST
 ************************************************************************/

#include "server.h"
void dis_list_sendMsg(pCLIENTNODE head, int cur_id, char *msg)
{
	while(head)
	{
		if(head->recv_fifo != cur_id)
		{
			write(head->send_fifo, msg, strlen(msg));
		}
		head = head->m_next;
	}
}
