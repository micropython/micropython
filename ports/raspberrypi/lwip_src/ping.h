#ifndef LWIP_PING_H
#define LWIP_PING_H

#include "lwip/raw.h"
#include "lwip/ip_addr.h"
#include "lwip/prot/icmp.h"

/**
 * PING_USE_SOCKETS: Set to 1 to use sockets, otherwise the raw api is used
 */
#ifndef PING_USE_SOCKETS
#define PING_USE_SOCKETS    LWIP_SOCKET
#endif

void ping_init(const ip_addr_t *ping_addr);
void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len);

extern u16_t ping_seq_num;
#if !PING_USE_SOCKETS
void ping_set_target(const ip_addr_t *ping_addr);
int ping_send(struct raw_pcb *raw, const ip_addr_t *addr);
// u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr);
#endif /* !PING_USE_SOCKETS */

#endif /* LWIP_PING_H */
