#ifndef MICROPY_INCLUDED_RA_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_RA_LWIP_LWIPOPTS_H

#define LWIP_IPV6                       0

#define LWIP_RAND() rng_read()

#define MEM_SIZE                        (16 * 1024)
#define TCP_MSS                         (1460)
#define TCP_OVERSIZE                    (TCP_MSS)
#define TCP_WND                         (8 * TCP_MSS)
#define TCP_SND_BUF                     (8 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * (TCP_SND_BUF / TCP_MSS))
#define TCP_QUEUE_OOSEQ                 (1)
#define MEMP_NUM_TCP_SEG                (2 * TCP_SND_QUEUELEN)

// Include common lwIP configuration.
#include "extmod/lwip-include/lwipopts_common.h"

extern uint32_t rng_read(void);

#endif // MICROPY_INCLUDED_RA_LWIP_LWIPOPTS_H
