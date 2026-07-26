#ifndef MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H

#include <stdint.h>

// lwIP needs a source of randomness (DHCP xid, TCP ISN, etc).  Use the port's
// pseudo-random number generator.
uint32_t mp_hal_get_pseudo_random_u32(void);
#define LWIP_RAND() mp_hal_get_pseudo_random_u32()

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

#endif // MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H
