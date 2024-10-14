#ifndef MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H
#define MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H

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
#define LWIP_RAW                        1
#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0
#define LWIP_STATS                      0
#define LWIP_NETIF_HOSTNAME             1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1

#define LWIP_LOOPIF_MULTICAST           1
#define LWIP_LOOPBACK_MAX_PBUFS         8

#define LWIP_IPV6                       0
#define LWIP_DHCP                       1
#define LWIP_DHCP_CHECK_LINK_UP         1
#define LWIP_DHCP_DOES_ACD_CHECK        0 // to speed DHCP up
#define LWIP_DNS                        1
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
#define LWIP_MDNS_RESPONDER             1
#define LWIP_IGMP                       1

#define LWIP_NUM_NETIF_CLIENT_DATA      LWIP_MDNS_RESPONDER
#define MEMP_NUM_UDP_PCB                (4 + LWIP_MDNS_RESPONDER)
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_NUM_SYS_TIMEOUT_INTERNAL + LWIP_MDNS_RESPONDER)

#define SO_REUSE                        1
#define TCP_LISTEN_BACKLOG              1

extern uint64_t se_services_rand64(void);
#define LWIP_RAND() se_services_rand64()

#define MEM_SIZE                        (16 * 1024)
#define TCP_MSS                         (1460)
#define TCP_OVERSIZE                    (TCP_MSS)
#define TCP_WND                         (8 * TCP_MSS)
#define TCP_SND_BUF                     (8 * TCP_MSS)
#define TCP_SND_QUEUELEN                (2 * (TCP_SND_BUF / TCP_MSS))
#define TCP_QUEUE_OOSEQ                 (1)
#define MEMP_NUM_TCP_SEG                (2 * TCP_SND_QUEUELEN)

typedef uint32_t sys_prot_t;

#endif // MICROPY_INCLUDED_ALIF_LWIP_LWIPOPTS_H
