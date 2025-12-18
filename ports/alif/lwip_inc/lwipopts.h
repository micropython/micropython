#ifndef MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_IPV6                       0

#define LWIP_RAND() se_services_rand64()

#define MEM_SIZE                        (16 * 1024)
#define TCP_MSS                         (1460)
#define TCP_OVERSIZE                    (TCP_MSS)
#define TCP_WND                         (8 * TCP_MSS)
#define TCP_SND_BUF                     (8 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * (TCP_SND_BUF / TCP_MSS))
#define TCP_QUEUE_OOSEQ                 (1)
#define MEMP_NUM_TCP_SEG                (2 * TCP_SND_QUEUELEN)

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

uint64_t se_services_rand64(void);

#endif // MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H
