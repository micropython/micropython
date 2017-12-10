#ifndef __ETHERNETIF_H__
#define __ETHERNETIF_H__


#include "lwip/err.h"
#include "lwip/netif.h"

/* Exported types ------------------------------------------------------------*/
err_t ethernetif_init(struct netif *netif);
void ethernetif_set_link(struct netif *netif);
void ethernetif_input(struct netif *netif);
err_t low_level_output(struct netif *netif, struct pbuf *p);
void ethernetif_update_config(struct netif *netif);
#endif
