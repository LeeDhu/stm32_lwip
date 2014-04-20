/**
 * @file lwipopts.h
 * @brief when transplant we need to modify it
 * @author Li Ming <Lee.dhucst@gmail.com>
 * @url http://blog.csdn.net/lim680
 * @version V1.0
 * @date 2014-04-13
 * @Copyright (c) DHU, CST.  All Rights Reserved
 */

#ifndef __LWIPOPTS_H
#define __LWIPOPTS_H

/* no operation system */
#define NO_SYS         1

/*
 * LWIP_SOCKET  --   socket api switches
 * LWIP_NETCONN --   sequential api switches
 */
#define LWIP_SOCKET     0
#define LWIP_NETCONN    0

#define MEM_ALIGN     4
#define MEM_SIZE          10*1024

#define TCP_SND_BUFF      2000

#define LWIP_ICMP                       1
/*
 *enable debug info 
 */
#define ETHARP_DEBUG    LWIP_DBG_ON
#define ICMP_DEBUG      LWIP_DBG_ON

#endif

