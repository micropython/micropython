#ifndef MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_IPV6                       0

#define LWIP_RAND() rng_get()

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t rng_get(void);

#endif // MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
