/**
 * @file tcp_server.c
 * @brief creat tcp server through RAW API
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-19
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#include "tcp_server.h"
#include "string.h"

static struct tcp_server server;
static const char *resp = "connect stm32 server successfully \r\n";

static err_t tcp_server_accept(void *arg, struct tcp_pcb *new_pcb, err_t err);
static err_t tcp_server_receive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
static void tcp_server_error(void *arg, err_t err);

void tcp_server_init(void)
{
	err_t status = ERR_OK;

	server.tcp_server_pcb = tcp_new();
	server.server_port = 5000;
	server.accept = tcp_server_accept;
	server.receive = tcp_server_receive;
	server.error  = tcp_server_error;
	if(server.tcp_server_pcb != NULL) {
		status = tcp_bind(server.tcp_server_pcb, IP_ADDR_ANY, server.server_port);
		if(status == ERR_OK) {
			server.tcp_server_pcb = tcp_listen(server.tcp_server_pcb);
			tcp_accept(server.tcp_server_pcb, server.accept);
			lwip_log("tcp server init ok \r\n");
		}
	}
}

static err_t tcp_server_accept(void *arg, struct tcp_pcb *new_pcb, err_t err)
{
	lwip_log("server accept \r\n");
	tcp_write(new_pcb, resp, strlen(resp), 1);
	tcp_err(new_pcb, server.error);
	tcp_recv(new_pcb, server.receive);

	return ERR_OK;

}


static err_t tcp_server_receive(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
	if(p != NULL) {
		tcp_recved(tpcb, p->tot_len);
		lwip_log("get data from client:%s \r\n", p->payload);
	}
	else if(err == ERR_OK) {
		return tcp_close(tpcb);
	}

	return ERR_OK;

}

static void tcp_server_error(void *arg, err_t err)
{


}
