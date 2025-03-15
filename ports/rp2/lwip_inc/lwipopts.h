#ifndef MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H

#include "py/mpconfig.h"

// This protection is not needed, instead protect lwIP code with flags
#define SYS_ARCH_DECL_PROTECT(lev) do { } while (0)
#define SYS_ARCH_PROTECT(lev) do { } while (0)
#define SYS_ARCH_UNPROTECT(lev) do { } while (0)

#define NO_SYS                          1
#define SYS_LIGHTWEIGHT_PROT            1
#define MEM_ALIGNMENT                   4

#define LWIP_CHKSUM_ALGORITHM           3
// The checksum flags are set in eth.c
#define LWIP_CHECKSUM_CTRL_PER_NETIF    1

#define LWIP_ARP                        1
#define LWIP_ETHERNET                   1
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_STATS                      0
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define LWIP_NETIF_STATUS_CALLBACK      1

#define LWIP_IPV4                       1
#define LWIP_IPV6                       1
#define LWIP_ND6_NUM_DESTINATIONS       4
#define LWIP_ND6_QUEUEING               0
#define LWIP_DHCP                       1
#define LWIP_DHCP_CHECK_LINK_UP         1
#define LWIP_DHCP_DOES_ACD_CHECK        0 // to speed DHCP up
#define LWIP_DNS                        1
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#define LWIP_MDNS_RESPONDER             1
#define LWIP_IGMP                       1

#if MICROPY_PY_LWIP_PPP
#define PPP_SUPPORT                     1
#define PAP_SUPPORT                     1
#define CHAP_SUPPORT                    1
#endif

#define LWIP_NUM_NETIF_CLIENT_DATA      LWIP_MDNS_RESPONDER
#define MEMP_NUM_UDP_PCB                (4 + LWIP_MDNS_RESPONDER)
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + LWIP_MDNS_RESPONDER)

#define SO_REUSE                        1
#define TCP_LISTEN_BACKLOG              1

extern uint32_t rosc_random_u32(void);
#define LWIP_RAND() rosc_random_u32()

// lwip takes 26700 bytes
#define MEM_SIZE (8000)
#define TCP_MSS (800)
#define TCP_WND (8 * TCP_MSS)
#define TCP_SND_BUF (8 * TCP_MSS)
#define MEMP_NUM_TCP_SEG (32)

typedef uint32_t sys_prot_t;

// Needed for PPP.
#define sys_jiffies sys_now

#endif // MICROPY_INCLUDED_RP2_LWIP_LWIPOPTS_H
