#ifndef MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H

#include <stdint.h>

// This protection is not needed, instead we execute all lwIP code at PendSV priority
#define SYS_ARCH_DECL_PROTECT(lev) do { } while (0)
#define SYS_ARCH_PROTECT(lev) do { } while (0)
#define SYS_ARCH_UNPROTECT(lev) do { } while (0)

#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT            1
#define MEM_ALIGNMENT                   4

#define LWIP_CHKSUM_ALGORITHM           3
#define LWIP_CHECKSUM_CTRL_PER_NETIF    1

#define LWIP_ARP                        1
#define LWIP_ETHERNET                   1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_STATS                      0
#define LWIP_NETIF_HOSTNAME             1

#define LWIP_IPV6                       0
#define LWIP_DHCP                       1
#define LWIP_DHCP_CHECK_LINK_UP         1
#define DHCP_DOES_ARP_CHECK             0 // to speed DHCP up
#define LWIP_DNS                        1
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#define LWIP_MDNS_RESPONDER             1
#define LWIP_IGMP                       1

#define LWIP_NUM_NETIF_CLIENT_DATA      1 // mDNS responder requires 1
#define MEMP_NUM_UDP_PCB                5 // mDNS responder requires 1

#define SO_REUSE                        1
#define TCP_LISTEN_BACKLOG              1

extern uint32_t rng_get(void);
#define LWIP_RAND() rng_get()

// default
// lwip takes 15800 bytes; TCP d/l: 380k/s local, 7.2k/s remote
// TCP u/l is very slow

#if 0
// lwip takes 19159 bytes; TCP d/l and u/l are around 320k/s on local network
#define MEM_SIZE (5000)
#define TCP_WND (4 * TCP_MSS)
#define TCP_SND_BUF (4 * TCP_MSS)
#endif

#if 1
// lwip takes 26700 bytes; TCP dl/ul are around 750/600 k/s on local network
#define MEM_SIZE (8000)
#define TCP_MSS (800)
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG (32)
#endif

#if 0
// lwip takes 45600 bytes; TCP dl/ul are around 1200/1000 k/s on local network
#define MEM_SIZE (16000)
#define TCP_MSS (1460)
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG (32)
#endif

typedef uint32_t sys_prot_t;

#endif // MICROPY_INCLUDED_STM32_LWIP_LWIPOPTS_H
