#ifndef MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define LWIP_NETIF_STATUS_CALLBACK      1

#define LWIP_IPV4                       1

#define LWIP_RAND() get_rand_32()

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

#include "pico/rand.h"

#endif // MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H
