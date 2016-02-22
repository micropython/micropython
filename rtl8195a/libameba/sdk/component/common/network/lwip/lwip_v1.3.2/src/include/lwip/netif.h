/**
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved. 
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 * 
 * Author: Adam Dunkels <adam@sics.se>
 *
 */
#ifndef __LWIP_NETIF_H__
#define __LWIP_NETIF_H__

#include "lwip/opt.h"

#define ENABLE_LOOPBACK (LWIP_NETIF_LOOPBACK || LWIP_HAVE_LOOPIF)

#include "lwip/err.h"

#include "lwip/ip_addr.h"

#include "lwip/inet.h"
#include "lwip/pbuf.h"
#if LWIP_DHCP
struct dhcp;
#endif
#if LWIP_AUTOIP
struct autoip;
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Throughout this file, IP addresses are expected to be in
 * the same byte order as in IP_PCB. */

/** must be the maximum of all used hardware address lengths
    across all types of interfaces in use */
#define NETIF_MAX_HWADDR_LEN 6U

/** TODO: define the use (where, when, whom) of netif flags */

/** whether the network interface is 'up'. this is
 * a software flag used to control whether this network
 * interface is enabled and processes traffic.
 */
#define NETIF_FLAG_UP           0x01U
/** if set, the netif has broadcast capability */
#define NETIF_FLAG_BROADCAST    0x02U
/** if set, the netif is one end of a point-to-point connection */
#define NETIF_FLAG_POINTTOPOINT 0x04U
/** if set, the interface is configured using DHCP */
#define NETIF_FLAG_DHCP         0x08U
/** if set, the interface has an active link
 *  (set by the network interface driver) */
#define NETIF_FLAG_LINK_UP      0x10U
/** if set, the netif is an device using ARP */
#define NETIF_FLAG_ETHARP       0x20U
/** if set, the netif has IGMP capability */
#define NETIF_FLAG_IGMP         0x40U
/** if set, the netif has IP layer switch capability */
/* Realtek Modified Start */
#ifdef CONFIG_DONT_CARE_TP
#define NETIF_FLAG_IPSWITCH       0x80U
#endif
/* Realtek Modified End */

/** Generic data structure used for all lwIP network interfaces.
 *  The following fields should be filled in by the initialization
 *  function for the device driver: hwaddr_len, hwaddr[], mtu, flags */

struct netif {
  /** pointer to next in linked list */
  struct netif *next;

  /** IP address configuration in network byte order */
  struct ip_addr ip_addr;
  struct ip_addr netmask;
  struct ip_addr gw;

  /** This function is called by the network device driver
   *  to pass a packet up the TCP/IP stack. */
  err_t (* input)(struct pbuf *p, struct netif *inp);
  /** This function is called by the IP module when it wants
   *  to send a packet on the interface. This function typically
   *  first resolves the hardware address, then sends the packet. */
  err_t (* output)(struct netif *netif, struct pbuf *p,
       struct ip_addr *ipaddr);
  /** This function is called by the ARP module when it wants
   *  to send a packet on the interface. This function outputs
   *  the pbuf as-is on the link medium. */
  err_t (* linkoutput)(struct netif *netif, struct pbuf *p);
#if LWIP_NETIF_STATUS_CALLBACK
  /** This function is called when the netif state is set to up or down
   */
  void (* status_callback)(struct netif *netif);
#endif /* LWIP_NETIF_STATUS_CALLBACK */
#if LWIP_NETIF_LINK_CALLBACK
  /** This function is called when the netif link is set to up or down
   */
  void (* link_callback)(struct netif *netif);
#endif /* LWIP_NETIF_LINK_CALLBACK */
  /** This field can be set by the device driver and could point
   *  to state information for the device. */
  void *state;
#if LWIP_DHCP
  /** the DHCP client state information for this netif */
  struct dhcp *dhcp;
#endif /* LWIP_DHCP */
#if LWIP_AUTOIP
  /** the AutoIP client state information for this netif */
  struct autoip *autoip;
#endif
#if LWIP_NETIF_HOSTNAME
  /* the hostname for this netif, NULL is a valid value */
  char*  hostname;
#endif /* LWIP_NETIF_HOSTNAME */
  /** maximum transfer unit (in bytes) */
  u16_t mtu;
  /** number of bytes used in hwaddr */
  u8_t hwaddr_len;
  /** link level hardware address of this interface */
  u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
  /** flags (see NETIF_FLAG_ above) */
  u8_t flags;
  /** descriptive abbreviation */
  char name[2];
  /** number of this interface */
  u8_t num;
#if LWIP_SNMP
  /** link type (from "snmp_ifType" enum from snmp.h) */
  u8_t link_type;
  /** (estimate) link speed */
  u32_t link_speed;
  /** timestamp at last change made (up/down) */
  u32_t ts;
  /** counters */
  u32_t ifinoctets;
  u32_t ifinucastpkts;
  u32_t ifinnucastpkts;
  u32_t ifindiscards;
  u32_t ifoutoctets;
  u32_t ifoutucastpkts;
  u32_t ifoutnucastpkts;
  u32_t ifoutdiscards;
#endif /* LWIP_SNMP */
#if LWIP_IGMP
  /* This function could be called to add or delete a entry in the multicast filter table of the ethernet MAC.*/
  err_t (*igmp_mac_filter)( struct netif *netif, struct ip_addr *group, u8_t action);
#endif /* LWIP_IGMP */
#if LWIP_NETIF_HWADDRHINT
  u8_t *addr_hint;
#endif /* LWIP_NETIF_HWADDRHINT */
#if ENABLE_LOOPBACK
  /* List of packets to be queued for ourselves. */
  struct pbuf *loop_first;
  struct pbuf *loop_last;
#if LWIP_LOOPBACK_MAX_PBUFS
  u16_t loop_cnt_current;
#endif /* LWIP_LOOPBACK_MAX_PBUFS */
#endif /* ENABLE_LOOPBACK */
};

#if LWIP_SNMP
#define NETIF_INIT_SNMP(netif, type, speed) \
  /* use "snmp_ifType" enum from snmp.h for "type", snmp_ifType_ethernet_csmacd by example */ \
  netif->link_type = type;    \
  /* your link speed here (units: bits per second) */  \
  netif->link_speed = speed;  \
  netif->ts = 0;              \
  netif->ifinoctets = 0;      \
  netif->ifinucastpkts = 0;   \
  netif->ifinnucastpkts = 0;  \
  netif->ifindiscards = 0;    \
  netif->ifoutoctets = 0;     \
  netif->ifoutucastpkts = 0;  \
  netif->ifoutnucastpkts = 0; \
  netif->ifoutdiscards = 0
#else /* LWIP_SNMP */
#define NETIF_INIT_SNMP(netif, type, speed)
#endif /* LWIP_SNMP */


/** The list of network interfaces. */
extern struct netif *netif_list;
/** The default network interface. */
extern struct netif *netif_default;

#define netif_init() /* Compatibility define, not init needed. */

struct netif *netif_add(struct netif *netif, struct ip_addr *ipaddr, struct ip_addr *netmask,
      struct ip_addr *gw,
      void *state,
      err_t (* init)(struct netif *netif),
      err_t (* input)(struct pbuf *p, struct netif *netif));

void
netif_set_addr(struct netif *netif,struct ip_addr *ipaddr, struct ip_addr *netmask,
    struct ip_addr *gw);
void netif_remove(struct netif * netif);

/* Returns a network interface given its name. The name is of the form
   "et0", where the first two letters are the "name" field in the
   netif structure, and the digit is in the num field in the same
   structure. */
struct netif *netif_find(char *name);

void netif_set_default(struct netif *netif);

void netif_set_ipaddr(struct netif *netif, struct ip_addr *ipaddr);
void netif_set_netmask(struct netif *netif, struct ip_addr *netmask);
void netif_set_gw(struct netif *netif, struct ip_addr *gw);

void netif_set_up(struct netif *netif);
void netif_set_down(struct netif *netif);
u8_t netif_is_up(struct netif *netif);

#if LWIP_NETIF_STATUS_CALLBACK
/*
 * Set callback to be called when interface is brought up/down
 */
void netif_set_status_callback(struct netif *netif, void (* status_callback)(struct netif *netif));
#endif /* LWIP_NETIF_STATUS_CALLBACK */

#if LWIP_NETIF_LINK_CALLBACK
void netif_set_link_up(struct netif *netif);
void netif_set_link_down(struct netif *netif);
u8_t netif_is_link_up(struct netif *netif);
/*
 * Set callback to be called when link is brought up/down
 */
void netif_set_link_callback(struct netif *netif, void (* link_callback)(struct netif *netif));
#endif /* LWIP_NETIF_LINK_CALLBACK */

#ifdef __cplusplus
}
#endif

#if ENABLE_LOOPBACK
err_t netif_loop_output(struct netif *netif, struct pbuf *p, struct ip_addr *dest_ip);
void netif_poll(struct netif *netif);
#if !LWIP_NETIF_LOOPBACK_MULTITHREADING
void netif_poll_all(void);
#endif /* !LWIP_NETIF_LOOPBACK_MULTITHREADING */
#endif /* ENABLE_LOOPBACK */

#endif /* __LWIP_NETIF_H__ */
