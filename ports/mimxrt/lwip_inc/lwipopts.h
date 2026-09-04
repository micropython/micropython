#ifndef MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_MIMXRT_LWIP_LWIPOPTS_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_IPV6                       0

#define LWIP_RAND() trng_random_u32()

// All boards using lwIP have ample OCRAM so use full-size TCP segments and windows.
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
