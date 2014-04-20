#include "stm32f10x.h"
#include "sys_arch.h"
#include "lwip/init.h"
#include "tcp.h"
#include "etharp.h"

struct netif enc28j60_netif;
/*MAC address*/
const u8 my_mac[6] = {0x04, 0x02, 0x35, 0x00, 0x00, 0x01};

extern vu32 local_ticks;
extern err_t ethernetif_init( struct netif *netif );
extern err_t ethernetif_input( struct netif *netif );

void net_interface_init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

#if LWIP_DHCP
	ipaddr.addr  = 0;
	netmask.addr = 0;
	gw.addr = 0;
#else
	IP4_ADDR(&ipaddr, 222, 204, 219, 253);
	IP4_ADDR(&netmask, 255, 255, 254, 0);
	IP4_ADDR(&gw, 222, 204, 218, 1);
#endif

	netif_add(&enc28j60_netif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &ethernet_input);
	netif_set_default(&enc28j60_netif);
#if LWIP_DHCP
	dhcp_start(&enc28j60_netif);
#endif

	netif_set_up(&enc28j60_netif);
}

u32_t sys_now(void)
{
	return local_ticks;
}

