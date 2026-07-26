#ifndef MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H

#include <stddef.h>
#include <stdint.h>

// lwIP needs a source of randomness (DHCP xid, TCP ISN, etc).  The qemu port
// only provides mp_hal_get_random(), so wrap it to return a 32-bit word.
#define LWIP_RAND() qemu_lwip_rand()

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

static inline uint32_t qemu_lwip_rand(void) {
    void mp_hal_get_random(size_t n, uint8_t *buf);
    uint32_t v;
    mp_hal_get_random(sizeof(v), (uint8_t *)&v);
    return v;
}

#endif // MICROPY_INCLUDED_QEMU_LWIP_LWIPOPTS_H
