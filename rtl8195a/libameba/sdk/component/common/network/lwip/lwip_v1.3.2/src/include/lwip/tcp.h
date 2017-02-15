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
#ifndef __LWIP_TCP_H__
#define __LWIP_TCP_H__

#include "lwip/opt.h"

#if LWIP_TCP /* don't build if not configured for use in lwipopts.h */

#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/ip.h"
#include "lwip/icmp.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif

struct tcp_pcb;

/* Functions for interfacing with TCP: */

/* Lower layer interface to TCP: */
#define tcp_init() /* Compatibility define, not init needed. */
void             tcp_tmr     (void);  /* Must be called every
                                         TCP_TMR_INTERVAL
                                         ms. (Typically 250 ms). */
/* Application program's interface: */
struct tcp_pcb * tcp_new     (void);
struct tcp_pcb * tcp_alloc   (u8_t prio);

void             tcp_arg     (struct tcp_pcb *pcb, void *arg);
void             tcp_accept  (struct tcp_pcb *pcb,
                              err_t (* accept)(void *arg, struct tcp_pcb *newpcb,
                 err_t err));
void             tcp_recv    (struct tcp_pcb *pcb,
                              err_t (* recv)(void *arg, struct tcp_pcb *tpcb,
                              struct pbuf *p, err_t err));
void             tcp_sent    (struct tcp_pcb *pcb,
                              err_t (* sent)(void *arg, struct tcp_pcb *tpcb,
                              u16_t len));
void             tcp_poll    (struct tcp_pcb *pcb,
                              err_t (* poll)(void *arg, struct tcp_pcb *tpcb),
                              u8_t interval);
void             tcp_err     (struct tcp_pcb *pcb,
                              void (* err)(void *arg, err_t err));

#define          tcp_mss(pcb)      ((pcb)->mss)
#define          tcp_sndbuf(pcb)   ((pcb)->snd_buf)
#define          tcp_nagle_disable(pcb)  ((pcb)->flags |= TF_NODELAY)
#define          tcp_nagle_enable(pcb) ((pcb)->flags &= ~TF_NODELAY)
#define          tcp_nagle_disabled(pcb) (((pcb)->flags & TF_NODELAY) != 0)

#if TCP_LISTEN_BACKLOG
#define          tcp_accepted(pcb) (((struct tcp_pcb_listen *)(pcb))->accepts_pending--)
#else  /* TCP_LISTEN_BACKLOG */
#define          tcp_accepted(pcb)
#endif /* TCP_LISTEN_BACKLOG */

void             tcp_recved  (struct tcp_pcb *pcb, u16_t len);
err_t            tcp_bind    (struct tcp_pcb *pcb, struct ip_addr *ipaddr,
                              u16_t port);
err_t            tcp_connect (struct tcp_pcb *pcb, struct ip_addr *ipaddr,
                              u16_t port, err_t (* connected)(void *arg,
                              struct tcp_pcb *tpcb,
                              err_t err));

struct tcp_pcb * tcp_listen_with_backlog(struct tcp_pcb *pcb, u8_t backlog);
#define          tcp_listen(pcb) tcp_listen_with_backlog(pcb, TCP_DEFAULT_LISTEN_BACKLOG)

void             tcp_abandon (struct tcp_pcb *pcb, int reset);
#define          tcp_abort(pcb) tcp_abandon((pcb), 1)
err_t            tcp_close   (struct tcp_pcb *pcb);

/* Flags for "apiflags" parameter in tcp_write and tcp_enqueue */
#define TCP_WRITE_FLAG_COPY 0x01
#define TCP_WRITE_FLAG_MORE 0x02

err_t            tcp_write   (struct tcp_pcb *pcb, const void *dataptr, u16_t len,
                              u8_t apiflags);

void             tcp_setprio (struct tcp_pcb *pcb, u8_t prio);

#define TCP_PRIO_MIN    1
#define TCP_PRIO_NORMAL 64
#define TCP_PRIO_MAX    127

/* It is also possible to call these two functions at the right
   intervals (instead of calling tcp_tmr()). */
void             tcp_slowtmr (void);
void             tcp_fasttmr (void);


/* Only used by IP to pass a TCP segment to TCP: */
void             tcp_input   (struct pbuf *p, struct netif *inp);
/* Used within the TCP code only: */
err_t            tcp_send_empty_ack(struct tcp_pcb *pcb);
err_t            tcp_output  (struct tcp_pcb *pcb);
void             tcp_rexmit  (struct tcp_pcb *pcb);
void             tcp_rexmit_rto  (struct tcp_pcb *pcb);
void             tcp_rexmit_fast (struct tcp_pcb *pcb);
u32_t            tcp_update_rcv_ann_wnd(struct tcp_pcb *pcb);

/**
 * This is the Nagle algorithm: try to combine user data to send as few TCP
 * segments as possible. Only send if
 * - no previously transmitted data on the connection remains unacknowledged or
 * - the TF_NODELAY flag is set (nagle algorithm turned off for this pcb) or
 * - the only unsent segment is at least pcb->mss bytes long (or there is more
 *   than one unsent segment - with lwIP, this can happen although unsent->len < mss)
 * - or if we are in fast-retransmit (TF_INFR)
 */
#define tcp_do_output_nagle(tpcb) ((((tpcb)->unacked == NULL) || \
                            ((tpcb)->flags & (TF_NODELAY | TF_INFR)) || \
                            (((tpcb)->unsent != NULL) && (((tpcb)->unsent->next != NULL) || \
                              ((tpcb)->unsent->len >= (tpcb)->mss))) \
                            ) ? 1 : 0)
#define tcp_output_nagle(tpcb) (tcp_do_output_nagle(tpcb) ? tcp_output(tpcb) : ERR_OK)


#define TCP_SEQ_LT(a,b)     ((s32_t)((a)-(b)) < 0)
#define TCP_SEQ_LEQ(a,b)    ((s32_t)((a)-(b)) <= 0)
#define TCP_SEQ_GT(a,b)     ((s32_t)((a)-(b)) > 0)
#define TCP_SEQ_GEQ(a,b)    ((s32_t)((a)-(b)) >= 0)
/* is b<=a<=c? */
#if 0 /* see bug #10548 */
#define TCP_SEQ_BETWEEN(a,b,c) ((c)-(b) >= (a)-(b))
#endif
#define TCP_SEQ_BETWEEN(a,b,c) (TCP_SEQ_GEQ(a,b) && TCP_SEQ_LEQ(a,c))
#define TCP_FIN 0x01U
#define TCP_SYN 0x02U
#define TCP_RST 0x04U
#define TCP_PSH 0x08U
#define TCP_ACK 0x10U
#define TCP_URG 0x20U
#define TCP_ECE 0x40U
#define TCP_CWR 0x80U

#define TCP_FLAGS 0x3fU

/* Length of the TCP header, excluding options. */
#define TCP_HLEN 20

#ifndef TCP_TMR_INTERVAL
#define TCP_TMR_INTERVAL       250  /* The TCP timer interval in milliseconds. */
#endif /* TCP_TMR_INTERVAL */

#ifndef TCP_FAST_INTERVAL
#define TCP_FAST_INTERVAL      TCP_TMR_INTERVAL /* the fine grained timeout in milliseconds */
#endif /* TCP_FAST_INTERVAL */

#ifndef TCP_SLOW_INTERVAL
#define TCP_SLOW_INTERVAL      (2*TCP_TMR_INTERVAL)  /* the coarse grained timeout in milliseconds */
#endif /* TCP_SLOW_INTERVAL */

#define TCP_FIN_WAIT_TIMEOUT 20000 /* milliseconds */
#define TCP_SYN_RCVD_TIMEOUT 20000 /* milliseconds */

#define TCP_OOSEQ_TIMEOUT        6U /* x RTO */

#ifndef TCP_MSL
#define TCP_MSL 60000UL /* The maximum segment lifetime in milliseconds */
#endif

/* Keepalive values, compliant with RFC 1122. Don't change this unless you know what you're doing */
#ifndef  TCP_KEEPIDLE_DEFAULT
#define  TCP_KEEPIDLE_DEFAULT     7200000UL /* Default KEEPALIVE timer in milliseconds */
#endif

#ifndef  TCP_KEEPINTVL_DEFAULT
#define  TCP_KEEPINTVL_DEFAULT    75000UL   /* Default Time between KEEPALIVE probes in milliseconds */
#endif

#ifndef  TCP_KEEPCNT_DEFAULT
#define  TCP_KEEPCNT_DEFAULT      9U        /* Default Counter for KEEPALIVE probes */
#endif

#define  TCP_MAXIDLE              TCP_KEEPCNT_DEFAULT * TCP_KEEPINTVL_DEFAULT  /* Maximum KEEPALIVE probe time */

/* Fields are (of course) in network byte order.
 * Some fields are converted to host byte order in tcp_input().
 */
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct tcp_hdr {
  PACK_STRUCT_FIELD(u16_t src);
  PACK_STRUCT_FIELD(u16_t dest);
  PACK_STRUCT_FIELD(u32_t seqno);
  PACK_STRUCT_FIELD(u32_t ackno);
  PACK_STRUCT_FIELD(u16_t _hdrlen_rsvd_flags);
  PACK_STRUCT_FIELD(u16_t wnd);
  PACK_STRUCT_FIELD(u16_t chksum);
  PACK_STRUCT_FIELD(u16_t urgp);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define TCPH_OFFSET(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 8)
#define TCPH_HDRLEN(phdr) (ntohs((phdr)->_hdrlen_rsvd_flags) >> 12)
#define TCPH_FLAGS(phdr)  (ntohs((phdr)->_hdrlen_rsvd_flags) & TCP_FLAGS)

#define TCPH_OFFSET_SET(phdr, offset) (phdr)->_hdrlen_rsvd_flags = htons(((offset) << 8) | TCPH_FLAGS(phdr))
#define TCPH_HDRLEN_SET(phdr, len) (phdr)->_hdrlen_rsvd_flags = htons(((len) << 12) | TCPH_FLAGS(phdr))
#define TCPH_FLAGS_SET(phdr, flags) (phdr)->_hdrlen_rsvd_flags = (((phdr)->_hdrlen_rsvd_flags & htons((u16_t)(~(u16_t)(TCP_FLAGS)))) | htons(flags))
#define TCPH_SET_FLAG(phdr, flags ) (phdr)->_hdrlen_rsvd_flags = ((phdr)->_hdrlen_rsvd_flags | htons(flags))
#define TCPH_UNSET_FLAG(phdr, flags) (phdr)->_hdrlen_rsvd_flags = htons(ntohs((phdr)->_hdrlen_rsvd_flags) | (TCPH_FLAGS(phdr) & ~(flags)) )

#define TCP_TCPLEN(seg) ((seg)->len + ((TCPH_FLAGS((seg)->tcphdr) & (TCP_FIN | TCP_SYN)) != 0))

enum tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};

/** Flags used on input processing, not on pcb->flags
*/
#define TF_RESET     (u8_t)0x08U   /* Connection was reset. */
#define TF_CLOSED    (u8_t)0x10U   /* Connection was sucessfully closed. */
#define TF_GOT_FIN   (u8_t)0x20U   /* Connection was closed by the remote end. */


#if LWIP_CALLBACK_API
  /* Function to call when a listener has been connected.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb a new tcp_pcb that now is connected
   * @param err an error argument (TODO: that is current always ERR_OK?)
   * @return ERR_OK: accept the new connection,
   *                 any other err_t abortsthe new connection
   */
#define DEF_ACCEPT_CALLBACK  err_t (* accept)(void *arg, struct tcp_pcb *newpcb, err_t err)
#else /* LWIP_CALLBACK_API */
#define DEF_ACCEPT_CALLBACK
#endif /* LWIP_CALLBACK_API */

/**
 * members common to struct tcp_pcb and struct tcp_listen_pcb
 */
#define TCP_PCB_COMMON(type) \
  type *next; /* for the linked list */ \
  enum tcp_state state; /* TCP state */ \
  u8_t prio; \
  void *callback_arg; \
  /* ports are in host byte order */ \
  u16_t local_port; \
  /* the accept callback for listen- and normal pcbs, if LWIP_CALLBACK_API */ \
  DEF_ACCEPT_CALLBACK


/* the TCP protocol control block */
struct tcp_pcb {
/** common PCB members */
  IP_PCB;
/** protocol specific PCB members */
  TCP_PCB_COMMON(struct tcp_pcb);

  /* ports are in host byte order */
  u16_t remote_port;
  
  u8_t flags;
#define TF_ACK_DELAY   ((u8_t)0x01U)   /* Delayed ACK. */
#define TF_ACK_NOW     ((u8_t)0x02U)   /* Immediate ACK. */
#define TF_INFR        ((u8_t)0x04U)   /* In fast recovery. */
#define TF_TIMESTAMP   ((u8_t)0x08U)   /* Timestamp option enabled */
#define TF_FIN         ((u8_t)0x20U)   /* Connection was closed locally (FIN segment enqueued). */
#define TF_NODELAY     ((u8_t)0x40U)   /* Disable Nagle algorithm */
#define TF_NAGLEMEMERR ((u8_t)0x80U)   /* nagle enabled, memerr, try to output to prevent delayed ACK to happen */

  /* the rest of the fields are in host byte order
     as we have to do some math with them */
  /* receiver variables */
  u32_t rcv_nxt;   /* next seqno expected */
  u16_t rcv_wnd;   /* receiver window available */
  u16_t rcv_ann_wnd; /* receiver window to announce */
  u32_t rcv_ann_right_edge; /* announced right edge of window */

  /* Timers */
  u32_t tmr;
  u8_t polltmr, pollinterval;
  
  /* Retransmission timer. */
  s16_t rtime;
  
  u16_t mss;   /* maximum segment size */
  
  /* RTT (round trip time) estimation variables */
  u32_t rttest; /* RTT estimate in 500ms ticks */
  u32_t rtseq;  /* sequence number being timed */
  s16_t sa, sv; /* @todo document this */

  s16_t rto;    /* retransmission time-out */
  u8_t nrtx;    /* number of retransmissions */

  /* fast retransmit/recovery */
  u32_t lastack; /* Highest acknowledged seqno. */
  u8_t dupacks;
  
  /* congestion avoidance/control variables */
  u16_t cwnd;  
  u16_t ssthresh;

  /* sender variables */
  u32_t snd_nxt;   /* next new seqno to be sent */
  u16_t snd_wnd;   /* sender window */
  u32_t snd_wl1, snd_wl2; /* Sequence and acknowledgement numbers of last
                             window update. */
  u32_t snd_lbb;       /* Sequence number of next byte to be buffered. */

  u16_t acked;
  
  u16_t snd_buf;   /* Available buffer space for sending (in bytes). */
#define TCP_SNDQUEUELEN_OVERFLOW (0xffff-3)
  u16_t snd_queuelen; /* Available buffer space for sending (in tcp_segs). */
  
  
  /* These are ordered by sequence number: */
  struct tcp_seg *unsent;   /* Unsent (queued) segments. */
  struct tcp_seg *unacked;  /* Sent but unacknowledged segments. */
#if TCP_QUEUE_OOSEQ  
  struct tcp_seg *ooseq;    /* Received out of sequence segments. */
#endif /* TCP_QUEUE_OOSEQ */

  struct pbuf *refused_data; /* Data previously received but not yet taken by upper layer */

#if LWIP_CALLBACK_API
  /* Function to be called when more send buffer space is available.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb the tcp_pcb which has send buffer space available
   * @param space the amount of bytes available
   * @return ERR_OK: try to send some data by calling tcp_output
   */
  err_t (* sent)(void *arg, struct tcp_pcb *pcb, u16_t space);
  
  /* Function to be called when (in-sequence) data has arrived.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb the tcp_pcb for which data has arrived
   * @param p the packet buffer which arrived
   * @param err an error argument (TODO: that is current always ERR_OK?)
   * @return ERR_OK: try to send some data by calling tcp_output
   */
  err_t (* recv)(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);

  /* Function to be called when a connection has been set up.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb the tcp_pcb that now is connected
   * @param err an error argument (TODO: that is current always ERR_OK?)
   * @return value is currently ignored
   */
  err_t (* connected)(void *arg, struct tcp_pcb *pcb, err_t err);

  /* Function which is called periodically.
   * The period can be adjusted in multiples of the TCP slow timer interval
   * by changing tcp_pcb.polltmr.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param pcb the tcp_pcb to poll for
   * @return ERR_OK: try to send some data by calling tcp_output
   */
  err_t (* poll)(void *arg, struct tcp_pcb *pcb);

  /* Function to be called whenever a fatal error occurs.
   * There is no pcb parameter since most of the times, the pcb is
   * already deallocated (or there is no pcb) when this function is called.
   * @param arg user-supplied argument (tcp_pcb.callback_arg)
   * @param err an indication why the error callback is called:
   *            ERR_ABRT: aborted through tcp_abort or by a TCP timer
   *            ERR_RST: the connection was reset by the remote host
   */
  void (* errf)(void *arg, err_t err);
#endif /* LWIP_CALLBACK_API */

#if LWIP_TCP_TIMESTAMPS
  u32_t ts_lastacksent;
  u32_t ts_recent;
#endif /* LWIP_TCP_TIMESTAMPS */

  /* idle time before KEEPALIVE is sent */
  u32_t keep_idle;
#if LWIP_TCP_KEEPALIVE
  u32_t keep_intvl;
  u32_t keep_cnt;
#endif /* LWIP_TCP_KEEPALIVE */
  
  /* Persist timer counter */
  u32_t persist_cnt;
  /* Persist timer back-off */
  u8_t persist_backoff;

  /* KEEPALIVE counter */
  u8_t keep_cnt_sent;
};

struct tcp_pcb_listen {  
/* Common members of all PCB types */
  IP_PCB;
/* Protocol specific PCB members */
  TCP_PCB_COMMON(struct tcp_pcb_listen);

#if TCP_LISTEN_BACKLOG
  u8_t backlog;
  u8_t accepts_pending;
#endif /* TCP_LISTEN_BACKLOG */
};

#if LWIP_EVENT_API

enum lwip_event {
  LWIP_EVENT_ACCEPT,
  LWIP_EVENT_SENT,
  LWIP_EVENT_RECV,
  LWIP_EVENT_CONNECTED,
  LWIP_EVENT_POLL,
  LWIP_EVENT_ERR
};

err_t lwip_tcp_event(void *arg, struct tcp_pcb *pcb,
         enum lwip_event,
         struct pbuf *p,
         u16_t size,
         err_t err);

#define TCP_EVENT_ACCEPT(pcb,err,ret)    ret = lwip_tcp_event((pcb)->callback_arg, (pcb),\
                LWIP_EVENT_ACCEPT, NULL, 0, err)
#define TCP_EVENT_SENT(pcb,space,ret) ret = lwip_tcp_event((pcb)->callback_arg, (pcb),\
                   LWIP_EVENT_SENT, NULL, space, ERR_OK)
#define TCP_EVENT_RECV(pcb,p,err,ret) ret = lwip_tcp_event((pcb)->callback_arg, (pcb),\
                LWIP_EVENT_RECV, (p), 0, (err))
#define TCP_EVENT_CONNECTED(pcb,err,ret) ret = lwip_tcp_event((pcb)->callback_arg, (pcb),\
                LWIP_EVENT_CONNECTED, NULL, 0, (err))
#define TCP_EVENT_POLL(pcb,ret)       ret = lwip_tcp_event((pcb)->callback_arg, (pcb),\
                LWIP_EVENT_POLL, NULL, 0, ERR_OK)
#define TCP_EVENT_ERR(errf,arg,err)  lwip_tcp_event((arg), NULL, \
                LWIP_EVENT_ERR, NULL, 0, (err))
#else /* LWIP_EVENT_API */

#define TCP_EVENT_ACCEPT(pcb,err,ret)                          \
  do {                                                         \
    if((pcb)->accept != NULL)                                  \
      (ret) = (pcb)->accept((pcb)->callback_arg,(pcb),(err));  \
    else (ret) = ERR_OK;                                       \
  } while (0)

#define TCP_EVENT_SENT(pcb,space,ret)                          \
  do {                                                         \
    if((pcb)->sent != NULL)                                    \
      (ret) = (pcb)->sent((pcb)->callback_arg,(pcb),(space));  \
    else (ret) = ERR_OK;                                       \
  } while (0)

#define TCP_EVENT_RECV(pcb,p,err,ret)                           \
  do {                                                          \
    if((pcb)->recv != NULL) {                                   \
      (ret) = (pcb)->recv((pcb)->callback_arg,(pcb),(p),(err)); \
    } else {                                                    \
      (ret) = tcp_recv_null(NULL, (pcb), (p), (err));           \
    }                                                           \
  } while (0)

#define TCP_EVENT_CONNECTED(pcb,err,ret)                         \
  do {                                                           \
    if((pcb)->connected != NULL)                                 \
      (ret) = (pcb)->connected((pcb)->callback_arg,(pcb),(err)); \
    else (ret) = ERR_OK;                                         \
  } while (0)

#define TCP_EVENT_POLL(pcb,ret)                                \
  do {                                                         \
    if((pcb)->poll != NULL)                                    \
      (ret) = (pcb)->poll((pcb)->callback_arg,(pcb));          \
    else (ret) = ERR_OK;                                       \
  } while (0)

#define TCP_EVENT_ERR(errf,arg,err)                            \
  do {                                                         \
    if((errf) != NULL)                                         \
      (errf)((arg),(err));                                     \
  } while (0)

#endif /* LWIP_EVENT_API */

/* This structure represents a TCP segment on the unsent and unacked queues */
struct tcp_seg {
  struct tcp_seg *next;    /* used when putting segements on a queue */
  struct pbuf *p;          /* buffer containing data + TCP header */
  void *dataptr;           /* pointer to the TCP data in the pbuf */
  u16_t len;               /* the TCP length of this segment */
  u8_t  flags;
#define TF_SEG_OPTS_MSS   (u8_t)0x01U   /* Include MSS option. */
#define TF_SEG_OPTS_TS    (u8_t)0x02U   /* Include timestamp option. */
  struct tcp_hdr *tcphdr;  /* the TCP header */
};

#define LWIP_TCP_OPT_LENGTH(flags)              \
  (flags & TF_SEG_OPTS_MSS ? 4  : 0) +          \
  (flags & TF_SEG_OPTS_TS  ? 12 : 0)

/** This returns a TCP header option for MSS in an u32_t */
#define TCP_BUILD_MSS_OPTION(x) (x) = htonl(((u32_t)2 << 24) |          \
                                            ((u32_t)4 << 16) |          \
                                            (((u32_t)TCP_MSS / 256) << 8) | \
                                            (TCP_MSS & 255))

/* Internal functions and global variables: */
struct tcp_pcb *tcp_pcb_copy(struct tcp_pcb *pcb);
void tcp_pcb_purge(struct tcp_pcb *pcb);
void tcp_pcb_remove(struct tcp_pcb **pcblist, struct tcp_pcb *pcb);

u8_t tcp_segs_free(struct tcp_seg *seg);
u8_t tcp_seg_free(struct tcp_seg *seg);
struct tcp_seg *tcp_seg_copy(struct tcp_seg *seg);

#define tcp_ack(pcb)                               \
  do {                                             \
    if((pcb)->flags & TF_ACK_DELAY) {              \
      (pcb)->flags &= ~TF_ACK_DELAY;               \
      (pcb)->flags |= TF_ACK_NOW;                  \
      tcp_output(pcb);                             \
    }                                              \
    else {                                         \
      (pcb)->flags |= TF_ACK_DELAY;                \
    }                                              \
  } while (0)

#define tcp_ack_now(pcb)                           \
  do {                                             \
    (pcb)->flags |= TF_ACK_NOW;                    \
    tcp_output(pcb);                               \
  } while (0)

err_t tcp_send_ctrl(struct tcp_pcb *pcb, u8_t flags);
err_t tcp_enqueue(struct tcp_pcb *pcb, void *dataptr, u16_t len,
                  u8_t flags, u8_t apiflags, u8_t optflags);

void tcp_rexmit_seg(struct tcp_pcb *pcb, struct tcp_seg *seg);

void tcp_rst(u32_t seqno, u32_t ackno,
       struct ip_addr *local_ip, struct ip_addr *remote_ip,
       u16_t local_port, u16_t remote_port);

u32_t tcp_next_iss(void);

void tcp_keepalive(struct tcp_pcb *pcb);
void tcp_zero_window_probe(struct tcp_pcb *pcb);

#if TCP_CALCULATE_EFF_SEND_MSS
u16_t tcp_eff_send_mss(u16_t sendmss, struct ip_addr *addr);
#endif /* TCP_CALCULATE_EFF_SEND_MSS */

#if LWIP_CALLBACK_API
err_t tcp_recv_null(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
#endif /* LWIP_CALLBACK_API */

extern struct tcp_pcb *tcp_input_pcb;
extern u32_t tcp_ticks;

const char* tcp_debug_state_str(enum tcp_state s);
#if TCP_DEBUG || TCP_INPUT_DEBUG || TCP_OUTPUT_DEBUG
void tcp_debug_print(struct tcp_hdr *tcphdr);
void tcp_debug_print_flags(u8_t flags);
void tcp_debug_print_state(enum tcp_state s);
void tcp_debug_print_pcbs(void);
s16_t tcp_pcbs_sane(void);
#else
#  define tcp_debug_print(tcphdr)
#  define tcp_debug_print_flags(flags)
#  define tcp_debug_print_state(s)
#  define tcp_debug_print_pcbs()
#  define tcp_pcbs_sane() 1
#endif /* TCP_DEBUG */

#if NO_SYS
#define tcp_timer_needed()
#else
void tcp_timer_needed(void);
#endif

/* The TCP PCB lists. */
union tcp_listen_pcbs_t { /* List of all TCP PCBs in LISTEN state. */
  struct tcp_pcb_listen *listen_pcbs; 
  struct tcp_pcb *pcbs;
};
extern union tcp_listen_pcbs_t tcp_listen_pcbs;
extern struct tcp_pcb *tcp_active_pcbs;  /* List of all TCP PCBs that are in a
              state in which they accept or send
              data. */
extern struct tcp_pcb *tcp_tw_pcbs;      /* List of all TCP PCBs in TIME-WAIT. */

extern struct tcp_pcb *tcp_tmp_pcb;      /* Only used for temporary storage. */

/* Axioms about the above lists:   
   1) Every TCP PCB that is not CLOSED is in one of the lists.
   2) A PCB is only in one of the lists.
   3) All PCBs in the tcp_listen_pcbs list is in LISTEN state.
   4) All PCBs in the tcp_tw_pcbs list is in TIME-WAIT state.
*/

/* Define two macros, TCP_REG and TCP_RMV that registers a TCP PCB
   with a PCB list or removes a PCB from a list, respectively. */
#if 0
#define TCP_REG(pcbs, npcb) do {\
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_REG %p local port %d\n", npcb, npcb->local_port)); \
                            for(tcp_tmp_pcb = *pcbs; \
          tcp_tmp_pcb != NULL; \
        tcp_tmp_pcb = tcp_tmp_pcb->next) { \
                                LWIP_ASSERT("TCP_REG: already registered\n", tcp_tmp_pcb != npcb); \
                            } \
                            LWIP_ASSERT("TCP_REG: pcb->state != CLOSED", npcb->state != CLOSED); \
                            npcb->next = *pcbs; \
                            LWIP_ASSERT("TCP_REG: npcb->next != npcb", npcb->next != npcb); \
                            *(pcbs) = npcb; \
                            LWIP_ASSERT("TCP_RMV: tcp_pcbs sane", tcp_pcbs_sane()); \
              tcp_timer_needed(); \
                            } while(0)
#define TCP_RMV(pcbs, npcb) do { \
                            LWIP_ASSERT("TCP_RMV: pcbs != NULL", *pcbs != NULL); \
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_RMV: removing %p from %p\n", npcb, *pcbs)); \
                            if(*pcbs == npcb) { \
                               *pcbs = (*pcbs)->next; \
                            } else for(tcp_tmp_pcb = *pcbs; tcp_tmp_pcb != NULL; tcp_tmp_pcb = tcp_tmp_pcb->next) { \
                               if(tcp_tmp_pcb->next == npcb) { \
                                  tcp_tmp_pcb->next = npcb->next; \
                                  break; \
                               } \
                            } \
                            npcb->next = NULL; \
                            LWIP_ASSERT("TCP_RMV: tcp_pcbs sane", tcp_pcbs_sane()); \
                            LWIP_DEBUGF(TCP_DEBUG, ("TCP_RMV: removed %p from %p\n", npcb, *pcbs)); \
                            } while(0)

#else /* LWIP_DEBUG */

#define TCP_REG(pcbs, npcb)                        \
  do {                                             \
    npcb->next = *pcbs;                            \
    *(pcbs) = npcb;                                \
    tcp_timer_needed();                            \
  } while (0)

#define TCP_RMV(pcbs, npcb)                        \
  do {                                             \
    if(*(pcbs) == npcb) {                          \
      (*(pcbs)) = (*pcbs)->next;                   \
    }                                              \
    else {                                         \
      for(tcp_tmp_pcb = *pcbs;                                         \
          tcp_tmp_pcb != NULL;                                         \
          tcp_tmp_pcb = tcp_tmp_pcb->next) {                           \
        if(tcp_tmp_pcb->next == npcb) {   \
          tcp_tmp_pcb->next = npcb->next;          \
          break;                                   \
        }                                          \
      }                                            \
    }                                              \
    npcb->next = NULL;                             \
  } while(0)

#endif /* LWIP_DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_TCP */

#endif /* __LWIP_TCP_H__ */
