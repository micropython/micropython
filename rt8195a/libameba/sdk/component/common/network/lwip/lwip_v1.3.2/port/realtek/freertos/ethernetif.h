#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

void ethernetif_recv(struct netif *netif, int total_len);
err_t ethernetif_init(struct netif *netif);
void lwip_PRE_SLEEP_PROCESSING(void);
void lwip_POST_SLEEP_PROCESSING(void);

#endif 
