/**
 * @file main.c
 * @brief 
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-13
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#include "mysys.h"
#include "usart.h"
#include "ticks.h"
#include "sys_arch.h"
#include "tcp_server.h"
#include "lwip.h"

#include "lwip/init.h"


int main()
{

	systick_init(SYS_TICKS);
	uart_init(115200);
	lwip_init();
	net_interface_init();
	tcp_server_init();
	printf("initial right\r\n");

	while(1) {
		lwip_poll();
	}
}
