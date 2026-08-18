#ifndef MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_IPV6                       0

#define LWIP_RAND() trng_random_u32()

// Use full-size TCP segments and windows: the lwipopts_common.h default
// tier (MSS=800, WND=8*MSS, MEM_SIZE=8000) caps TCP at single-digit
// Mbit/s on WiFi links.  Only boards with a network interface compile
// lwIP on this port, and all of them have ample OCRAM for these sizes.
// The heap is deliberately larger than TCP_SND_BUF so lwIP's ERR_MEM
// retry path stays cold, and the pool covers the receive window.
// Boards can override any of these before this point if needed.
#ifndef MEM_SIZE
#define MEM_SIZE                        (13 * 1024)
#define TCP_MSS                         (1460)
#define TCP_WND                         (7 * TCP_MSS)
#define TCP_SND_BUF                     (6 * TCP_MSS)
#define MEMP_NUM_TCP_SEG                (24)
#define PBUF_POOL_SIZE                  (7)
#endif

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t trng_random_u32(void);

#endif // MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
