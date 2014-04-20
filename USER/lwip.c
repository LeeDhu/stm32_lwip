#include "lwip.h"
#include "lwip/tcp_impl.h"
#include "lwip/ip_frag.h"
#include "netif/etharp.h"
#include "lwip/netif.h"


static u32 last_tcp_time = 0;
static u32 last_arp_time = 0;
static u32 last_ipreass_time = 0;
#if LWIP_DHCP
static u32 last_dhcp_fine_time = 0;
static u32 last_dhcp_coarse_time = 0;
#endif

extern struct netif enc28j60_netif;
extern void ethernetif_input(struct netif *netif);

static u8 time_expired(u32 *last_time, u32 interval)
{
	if((local_ticks - *last_time) >= interval) {
		*last_time = local_ticks;
		return 1;
	}

	return 0;
}

void lwip_poll(void)
{
	ethernetif_input(&enc28j60_netif);

	if(time_expired(&last_tcp_time, TCP_TMR_INTERVAL)) {
		tcp_tmr();
	}

	if(time_expired(&last_arp_time, ARP_TMR_INTERVAL)) {
		etharp_tmr();
	}

	if(time_expired(&last_ipreass_time, IP_TMR_INTERVAL)) {
		ip_reass_tmr();
	}

#if LWIP_DHCP
	if(time_expired(&last_dhcp_fine_time, DHCP_FINE_TIMER_MSECS)) {
		dhcp_fine_tmr();
	}
	if(time_expired(&last_dhcp_coarse_time, DHCP_COARSE_TIMER_MSECS)) {
		dhcp_coarse_tmr();
	}
#endif	

}
