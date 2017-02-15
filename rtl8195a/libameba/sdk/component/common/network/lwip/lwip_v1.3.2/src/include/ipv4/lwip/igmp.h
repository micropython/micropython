/**
 * Copyright (c) 2002 CITEL Technologies Ltd.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met: 
 * 1. Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer. 
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the distribution. 
 * 3. Neither the name of CITEL Technologies Ltd nor the names of its contributors 
 *    may be used to endorse or promote products derived from this software 
 *    without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY CITEL TECHNOLOGIES AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
 * ARE DISCLAIMED.  IN NO EVENT SHALL CITEL TECHNOLOGIES OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS 
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY 
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF 
 * SUCH DAMAGE. 
 *
 * This file is a contribution to the lwIP TCP/IP stack.
 * The Swedish Institute of Computer Science and Adam Dunkels
 * are specifically granted permission to redistribute this
 * source code.
**/

#ifndef __LWIP_IGMP_H__
#define __LWIP_IGMP_H__

#include "lwip/opt.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"

#if LWIP_IGMP /* don't build if not configured for use in lwipopts.h */

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * IGMP constants
 */
#define IP_PROTO_IGMP                  2
#define IGMP_TTL                       1
#define IGMP_MINLEN                    8
#define ROUTER_ALERT                   0x9404
#define ROUTER_ALERTLEN                4

/*
 * IGMP message types, including version number.
 */
#define IGMP_MEMB_QUERY                0x11 /* Membership query         */
#define IGMP_V1_MEMB_REPORT            0x12 /* Ver. 1 membership report */
#define IGMP_V2_MEMB_REPORT            0x16 /* Ver. 2 membership report */
#define IGMP_LEAVE_GROUP               0x17 /* Leave-group message      */

/* IGMP timer */
#define IGMP_TMR_INTERVAL              100 /* Milliseconds */
#define IGMP_V1_DELAYING_MEMBER_TMR   (1000/IGMP_TMR_INTERVAL)
#define IGMP_JOIN_DELAYING_MEMBER_TMR (500 /IGMP_TMR_INTERVAL)

/* MAC Filter Actions */
#define IGMP_DEL_MAC_FILTER            0
#define IGMP_ADD_MAC_FILTER            1

/* Group  membership states */
#define IGMP_GROUP_NON_MEMBER          0
#define IGMP_GROUP_DELAYING_MEMBER     1
#define IGMP_GROUP_IDLE_MEMBER         2

/*
 * IGMP packet format.
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct igmp_msg {
 PACK_STRUCT_FIELD(u8_t           igmp_msgtype);
 PACK_STRUCT_FIELD(u8_t           igmp_maxresp);
 PACK_STRUCT_FIELD(u16_t          igmp_checksum);
 PACK_STRUCT_FIELD(struct ip_addr igmp_group_address);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

/* 
 * now a group structure - there is
 * a list of groups for each interface
 * these should really be linked from the interface, but
 * if we keep them separate we will not affect the lwip original code
 * too much
 * 
 * There will be a group for the all systems group address but this 
 * will not run the state machine as it is used to kick off reports
 * from all the other groups
 */

struct igmp_group {
  struct igmp_group *next;
  struct netif      *interface;
  struct ip_addr     group_address;
  u8_t               last_reporter_flag; /* signifies we were the last person to report */
  u8_t               group_state;
  u16_t              timer;
  u8_t               use; /* counter of simultaneous uses */
};


/*  Prototypes */
void   igmp_init(void);

err_t  igmp_start( struct netif *netif);

err_t  igmp_stop( struct netif *netif);

void   igmp_report_groups( struct netif *netif);

struct igmp_group *igmp_lookfor_group( struct netif *ifp, struct ip_addr *addr);

struct igmp_group *igmp_lookup_group( struct netif *ifp, struct ip_addr *addr);

err_t  igmp_remove_group( struct igmp_group *group);

void   igmp_input( struct pbuf *p, struct netif *inp, struct ip_addr *dest);

err_t  igmp_joingroup( struct ip_addr *ifaddr, struct ip_addr *groupaddr);

err_t  igmp_leavegroup( struct ip_addr *ifaddr, struct ip_addr *groupaddr);

void   igmp_tmr(void);

void   igmp_timeout( struct igmp_group *group);

void   igmp_start_timer( struct igmp_group *group, u8_t max_time);

void   igmp_stop_timer( struct igmp_group *group);

void   igmp_delaying_member( struct igmp_group *group, u8_t maxresp);

err_t  igmp_ip_output_if( struct pbuf *p, struct ip_addr *src, struct ip_addr *dest, u8_t ttl, u8_t proto, struct netif *netif);

void   igmp_send( struct igmp_group *group, u8_t type);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_IGMP */

#endif /* __LWIP_IGMP_H__ */
