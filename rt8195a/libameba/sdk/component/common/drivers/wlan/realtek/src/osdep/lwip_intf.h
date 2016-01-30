#ifndef __LWIP_INTF_H__
#define __LWIP_INTF_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include <wireless.h>
#include <skbuff.h>

//----- ------------------------------------------------------------------
// Ethernet Buffer
//----- ------------------------------------------------------------------
struct eth_drv_sg {
    unsigned int	buf;
    unsigned int 	len;
};

#define MAX_ETH_DRV_SG	32
#define MAX_ETH_MSG	1540

//----- ------------------------------------------------------------------
// Wlan Interface Provided
//----- ------------------------------------------------------------------
unsigned char rltk_wlan_check_isup(int idx);
void rltk_wlan_tx_inc(int idx);
void rltk_wlan_tx_dec(int idx);
struct sk_buff * rltk_wlan_get_recv_skb(int idx);
struct sk_buff * rltk_wlan_alloc_skb(unsigned int total_len);
void rltk_wlan_set_netif_info(int idx_wlan, void * dev, unsigned char * dev_addr);
void rltk_wlan_send_skb(int idx, struct sk_buff *skb);	//struct sk_buff as defined above comment line
int rltk_wlan_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len);
void rltk_wlan_recv(int idx, struct eth_drv_sg *sg_list, int sg_len);
unsigned char rltk_wlan_running(unsigned char idx);		// interface is up. 0: interface is down

//----- ------------------------------------------------------------------
// Network Interface provided
//----- ------------------------------------------------------------------
struct netif;
int netif_is_valid_IP(int idx,unsigned char * ip_dest);
int netif_get_idx(struct netif *pnetif);
unsigned char *netif_get_hwaddr(int idx_wlan);
void netif_rx(int idx, unsigned int len);
void netif_post_sleep_processing(void);
void netif_pre_sleep_processing(void);
#if (CONFIG_LWIP_LAYER == 1)
extern void ethernetif_recv(struct netif *netif, int total_len);
extern void lwip_PRE_SLEEP_PROCESSING(void);
extern void lwip_POST_SLEEP_PROCESSING(void);
#endif //CONFIG_LWIP_LAYER == 1

#ifdef	__cplusplus
}
#endif

#endif //#ifndef __LWIP_INTF_H__
