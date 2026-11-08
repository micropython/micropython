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

// Trim features this port cannot use, to make room for netif_add() and the TCP
// path in the 448 KB code region (the image is within a few KB of the limit).
//   * the mDNS responder is not usable here and is several KB of code
//   * mDNS queries through the resolver go with the responder
// IGMP has to stay enabled: modlwip.c calls igmp_joingroup()/igmp_leavegroup()
// unconditionally for the IP_ADD_MEMBERSHIP socket option.
// Disabled after the shared options are included because that header defines
// them unconditionally.
#undef LWIP_MDNS_RESPONDER
#define LWIP_MDNS_RESPONDER             0
#undef LWIP_DNS_SUPPORT_MDNS_QUERIES
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   0

#endif // MICROPY_INCLUDED_GD32_LWIPOPTS_H
