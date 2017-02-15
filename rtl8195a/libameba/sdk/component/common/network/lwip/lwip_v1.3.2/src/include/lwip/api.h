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
 **/
#ifndef __LWIP_API_H__
#define __LWIP_API_H__

#include "lwip/opt.h"

#if LWIP_NETCONN /* don't build if not configured for use in lwipopts.h */

#include <stddef.h> /* for size_t */

#include "lwip/netbuf.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

// Evan add for port netconn api
#define port_netconn_recv(conn , buf, ret)  do{buf = netconn_recv(conn);}while(0);
#define port_netconn_accept(conn , newconn, ret) do{newconn = netconn_accept(conn);}while(0);
// Evan add end

/* Throughout this file, IP addresses and port numbers are expected to be in
 * the same byte order as in the corresponding pcb.
 */

/* Flags for netconn_write */
#define NETCONN_NOFLAG 0x00
#define NETCONN_NOCOPY 0x00 /* Only for source code compatibility */
#define NETCONN_COPY   0x01
#define NETCONN_MORE   0x02

/* Helpers to process several netconn_types by the same code */
#define NETCONNTYPE_GROUP(t)    (t&0xF0)
#define NETCONNTYPE_DATAGRAM(t) (t&0xE0)

enum netconn_type {
  NETCONN_INVALID    = 0,
  /* NETCONN_TCP Group */
  NETCONN_TCP        = 0x10,
  /* NETCONN_UDP Group */
  NETCONN_UDP        = 0x20,
  NETCONN_UDPLITE    = 0x21,
  NETCONN_UDPNOCHKSUM= 0x22,
  /* NETCONN_RAW Group */
  NETCONN_RAW        = 0x40
};

enum netconn_state {
  NETCONN_NONE,
  NETCONN_WRITE,
  NETCONN_LISTEN,
  NETCONN_CONNECT,
  NETCONN_CLOSE
};

enum netconn_evt {
  NETCONN_EVT_RCVPLUS,
  NETCONN_EVT_RCVMINUS,
  NETCONN_EVT_SENDPLUS,
  NETCONN_EVT_SENDMINUS
};

#if LWIP_IGMP
enum netconn_igmp {
  NETCONN_JOIN,
  NETCONN_LEAVE
};
#endif /* LWIP_IGMP */

/* forward-declare some structs to avoid to include their headers */
struct ip_pcb;
struct tcp_pcb;
struct udp_pcb;
struct raw_pcb;
struct netconn;

/** A callback prototype to inform about events for a netconn */
typedef void (* netconn_callback)(struct netconn *, enum netconn_evt, u16_t len);

/** A netconn descriptor */
struct netconn {
  /** type of the netconn (TCP, UDP or RAW) */
  enum netconn_type type;
  /** current state of the netconn */
  enum netconn_state state;
  /** the lwIP internal protocol control block */
  union {
    struct ip_pcb  *ip;
    struct tcp_pcb *tcp;
    struct udp_pcb *udp;
    struct raw_pcb *raw;
  } pcb;
  /** the last error this netconn had */
  err_t err;
  /** sem that is used to synchroneously execute functions in the core context */
  sys_sem_t op_completed;
  /** mbox where received packets are stored until they are fetched
      by the netconn application thread (can grow quite big) */
  sys_mbox_t recvmbox;
  /** mbox where new connections are stored until processed
      by the application thread */
  sys_mbox_t acceptmbox;
  /** only used for socket layer */
  int socket;
#if LWIP_SO_RCVTIMEO
  /** timeout to wait for new data to be received
      (or connections to arrive for listening netconns) */
  int recv_timeout;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
  /** maximum amount of bytes queued in recvmbox */
  int recv_bufsize;
#endif /* LWIP_SO_RCVBUF */
  s16_t recv_avail;
#if LWIP_TCP
  /** TCP: when data passed to netconn_write doesn't fit into the send buffer,
      this temporarily stores the message. */
  struct api_msg_msg *write_msg;
  /** TCP: when data passed to netconn_write doesn't fit into the send buffer,
      this temporarily stores how much is already sent. */
  size_t write_offset;
#if LWIP_TCPIP_CORE_LOCKING
  /** TCP: when data passed to netconn_write doesn't fit into the send buffer,
      this temporarily stores whether to wake up the original application task
      if data couldn't be sent in the first try. */
  u8_t write_delayed;
#endif /* LWIP_TCPIP_CORE_LOCKING */
#endif /* LWIP_TCP */
  /** A callback function that is informed about events for this netconn */
  netconn_callback callback;
};

/* Register an Network connection event */
#define API_EVENT(c,e,l) if (c->callback) {         \
                           (*c->callback)(c, e, l); \
                         }

/* Network connection functions: */
#define netconn_new(t)                  netconn_new_with_proto_and_callback(t, 0, NULL)
#define netconn_new_with_callback(t, c) netconn_new_with_proto_and_callback(t, 0, c)
struct
netconn *netconn_new_with_proto_and_callback(enum netconn_type t, u8_t proto,
                                   netconn_callback callback);
err_t             netconn_delete  (struct netconn *conn);
/** Get the type of a netconn (as enum netconn_type). */
#define netconn_type(conn) (conn->type)

err_t             netconn_getaddr (struct netconn *conn,
                                   struct ip_addr *addr,
                                   u16_t *port,
                                   u8_t local);
#define netconn_peer(c,i,p) netconn_getaddr(c,i,p,0)
#define netconn_addr(c,i,p) netconn_getaddr(c,i,p,1)

err_t             netconn_bind    (struct netconn *conn,
                                   struct ip_addr *addr,
                                   u16_t port);
err_t             netconn_connect (struct netconn *conn,
                                   struct ip_addr *addr,
                                   u16_t port);
err_t             netconn_disconnect (struct netconn *conn);
err_t             netconn_listen_with_backlog(struct netconn *conn, u8_t backlog);
#define netconn_listen(conn) netconn_listen_with_backlog(conn, TCP_DEFAULT_LISTEN_BACKLOG)
struct netconn *  netconn_accept  (struct netconn *conn);
struct netbuf *   netconn_recv    (struct netconn *conn);
err_t             netconn_sendto  (struct netconn *conn,
                                   struct netbuf *buf, struct ip_addr *addr, u16_t port);
err_t             netconn_send    (struct netconn *conn,
                                   struct netbuf *buf);
err_t             netconn_write   (struct netconn *conn,
                                   const void *dataptr, size_t size,
                                   u8_t apiflags);
err_t             netconn_close   (struct netconn *conn);

err_t             netconn_abort(struct netconn *conn);

#if LWIP_IGMP
err_t             netconn_join_leave_group (struct netconn *conn,
                                            struct ip_addr *multiaddr,
                                            struct ip_addr *interface,
                                            enum netconn_igmp join_or_leave);
#endif /* LWIP_IGMP */
#if LWIP_DNS
err_t             netconn_gethostbyname(const char *name, struct ip_addr *addr);
#endif /* LWIP_DNS */

#define netconn_err(conn)          ((conn)->err)
#define netconn_recv_bufsize(conn) ((conn)->recv_bufsize)

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETCONN */

#endif /* __LWIP_API_H__ */
