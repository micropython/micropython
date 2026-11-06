// lwipopts.h - lwIP configuration for the GD32F4xx port.
#ifndef MICROPY_INCLUDED_GD32_LWIPOPTS_H
#define MICROPY_INCLUDED_GD32_LWIPOPTS_H

// Provide lwIP's port hooks (sys_arch, byte order, types).
#include "arch/cc.h"

// Random numbers come from the port's hardware RNG.
uint32_t mp_hal_get_hw_random_u32(void);
#define LWIP_RAND() mp_hal_get_hw_random_u32()

// Keep the loopback interface available so sockets tests can run without a PHY.
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

// Modest memory footprint: the GD32F407 build only has ~50 KB of RAM left.
#define MEM_SIZE                        (16 * 1024)
#define PBUF_POOL_SIZE                  (8)
#define MEMP_NUM_TCP_SEG                (16)

#include "extmod/lwip-include/lwipopts_common.h"

#endif // MICROPY_INCLUDED_GD32_LWIPOPTS_H
