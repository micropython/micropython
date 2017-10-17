#ifndef STM32_LWIPOPTS_H
#define STM32_LWIPOPTS_H

#include <stdint.h>

#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT            1

#define LWIP_ARP                        1
#define LWIP_ETHERNET                   1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_STATS                      0

#define LWIP_DHCP                       1
#define LWIP_DNS                        1
#define LWIP_IGMP                       1

#define SO_REUSE                        1

extern uint32_t rng_get(void);
#define LWIP_RAND()                     rng_get()

// default
// lwip takes 15800 bytes; TCP d/l: 380k/s local, 7.2k/s remote

#if 0
// lwip takes 26700 bytes; TCP d/l: 530k/s local, 21k/s remote
#define MEM_SIZE (8000)
#define TCP_MSS (800)
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG (32)
#endif

#if 0
// lwip takes 45600 bytes; TCP d/l: 720k/s local, 36k/s remote
#define MEM_SIZE (16000)
#define TCP_MSS (1460)
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG (32)
#endif

typedef uint32_t sys_prot_t;

#endif // STM32_LWIPOPTS_H
