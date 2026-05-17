#ifndef MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_IPV6                       0

#define LWIP_RAND() trng_random_u32()

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t trng_random_u32(void);

#endif // MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
