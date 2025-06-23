#ifndef MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_IPV6                       0

#define LWIP_RAND() rng_get()

// Increase memory for lwIP to get better performance.
#if defined(STM32N6)
#define MEM_SIZE                        (16 * 1024)
#define TCP_MSS                         (1460)
#define TCP_WND                         (8 * TCP_MSS)
#define TCP_SND_BUF                     (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG                (32)
#endif

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t rng_get(void);

#endif // MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
