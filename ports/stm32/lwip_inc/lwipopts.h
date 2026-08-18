#ifndef MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H

#include STM32_HAL_H

#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_RAND() mp_hal_get_hw_random_u32()

// Increase memory for lwIP to get better performance.
#if defined(STM32N6)
#define MEM_SIZE                        (64 * 1024)
#define PBUF_POOL_SIZE                  (32)
#define TCP_MSS                         (1460)
// The receive window is sized for WiFi round-trip times: 16*MSS caps TCP
// receive at window/RTT (~31Mbit/s at the ~6ms RTT of a CYW43439 link),
// measurably below what the link carries.  32*MSS is covered by the
// existing PBUF_POOL with no additional static cost.
#define TCP_WND                         (32 * TCP_MSS)
#define TCP_SND_BUF                     (16 * TCP_MSS)
#define MEMP_NUM_TCP_SEG                (64)
#endif

// Include common lwIP configuration (also mpconfig.h).
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t mp_hal_get_hw_random_u32(void);

#endif // MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
