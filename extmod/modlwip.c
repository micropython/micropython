/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2015 Galen Hazelwood
 * Copyright (c) 2015-2017 Paul Sokolovsky
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>
#include <stdio.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_LWIP

#include "shared/netutils/netutils.h"
#include "modnetwork.h"

#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/raw.h"
#include "lwip/dns.h"
#include "lwip/igmp.h"
#if LWIP_VERSION_MAJOR < 2
#include "lwip/timers.h"
#include "lwip/tcp_impl.h"
#else
#include "lwip/timeouts.h"
#include "lwip/priv/tcp_priv.h"
#endif

#if 0 // print debugging info
#define DEBUG_printf DEBUG_printf
#else // don't print debugging info
#define DEBUG_printf(...) (void)0
#endif

// Timeout between closing a TCP socket and doing a tcp_abort on that
// socket, if the connection isn't closed cleanly in that time.
#define MICROPY_PY_LWIP_TCP_CLOSE_TIMEOUT_MS (10000)

// All socket options should be globally distinct,
// because we ignore option levels for efficiency.
#define IP_ADD_MEMBERSHIP 0x400
#define IP_DROP_MEMBERSHIP 0x401

#define TCP_NODELAY TF_NODELAY

// For compatibilily with older lwIP versions.
#ifndef ip_set_option
#define ip_set_option(pcb, opt)   ((pcb)->so_options |= (opt))
#endif
#ifndef ip_reset_option
#define ip_reset_option(pcb, opt) ((pcb)->so_options &= ~(opt))
#endif
#ifndef tcp_set_flags
#define tcp_set_flags(pcb, set_flags) do { (pcb)->flags |= (set_flags); } while (0)
#endif
#ifndef tcp_clear_flags
#define tcp_clear_flags(pcb, clear_flags) do { (pcb)->flags &= ~(clear_flags); } while (0)
#endif

// A port can define these hooks to provide concurrency protection
#ifndef MICROPY_PY_LWIP_ENTER
#define MICROPY_PY_LWIP_ENTER
#define MICROPY_PY_LWIP_REENTER
#define MICROPY_PY_LWIP_EXIT
#endif

#ifdef MICROPY_PY_LWIP_SLIP
#include "netif/slipif.h"
#include "lwip/sio.h"
#endif

#ifdef MICROPY_PY_LWIP_SLIP
/******************************************************************************/
// Slip object for modlwip. Requires a serial driver for the port that supports
// the lwip serial callback functions.

typedef struct _lwip_slip_obj_t {
    mp_obj_base_t base;
    struct netif lwip_netif;
} lwip_slip_obj_t;

// Slip object is unique for now. Possibly can fix this later. FIXME
static lwip_slip_obj_t lwip_slip_obj;

// Declare these early.
void mod_lwip_register_poll(void (*poll)(void *arg), void *poll_arg);
void mod_lwip_deregister_poll(void (*poll)(void *arg), void *poll_arg);

static void slip_lwip_poll(void *netif) {
    slipif_poll((struct netif *)netif);
}

static const mp_obj_type_t lwip_slip_type;

// lwIP SLIP callback functions
sio_fd_t sio_open(u8_t dvnum) {
    // We support singleton SLIP interface, so just return any truish value.
    return (sio_fd_t)1;
}

void sio_send(u8_t c, sio_fd_t fd) {
    const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(lwip_slip_stream));
    int error;
    stream_p->write(MP_STATE_VM(lwip_slip_stream), &c, 1, &error);
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len) {
    const mp_stream_p_t *stream_p = mp_get_stream(MP_STATE_VM(lwip_slip_stream));
    int error;
    mp_uint_t out_sz = stream_p->read(MP_STATE_VM(lwip_slip_stream), data, len, &error);
    if (out_sz == MP_STREAM_ERROR) {
        if (mp_is_nonblocking_error(error)) {
            return 0;
        }
        // Can't do much else, can we?
        return 0;
    }
    return out_sz;
}

// constructor lwip.slip(device=integer, iplocal=string, ipremote=string)
static mp_obj_t lwip_slip_make_new(mp_obj_t type_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    lwip_slip_obj.base.type = &lwip_slip_type;

    MP_STATE_VM(lwip_slip_stream) = args[0];

    ip_addr_t iplocal, ipremote;
    if (!ipaddr_aton(mp_obj_str_get_str(args[1]), &iplocal)) {
        mp_raise_ValueError(MP_ERROR_TEXT("not a valid local IP"));
    }
    if (!ipaddr_aton(mp_obj_str_get_str(args[2]), &ipremote)) {
        mp_raise_ValueError(MP_ERROR_TEXT("not a valid remote IP"));
    }

    struct netif *n = &lwip_slip_obj.lwip_netif;
    if (netif_add(n, &iplocal, IP_ADDR_BROADCAST, &ipremote, NULL, slipif_init, ip_input) == NULL) {
        mp_raise_ValueError(MP_ERROR_TEXT("out of memory"));
    }
    netif_set_up(n);
    netif_set_default(n);
    mod_lwip_register_poll(slip_lwip_poll, n);

    return (mp_obj_t)&lwip_slip_obj;
}

static mp_obj_t lwip_slip_status(mp_obj_t self_in) {
    // Null function for now.
    return mp_const_none;
}

static MP_DEFINE_CONST_FUN_OBJ_1(lwip_slip_status_obj, lwip_slip_status);

static const mp_rom_map_elem_t lwip_slip_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&lwip_slip_status_obj) },
};

static MP_DEFINE_CONST_DICT(lwip_slip_locals_dict, lwip_slip_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    lwip_slip_type,
    MP_QSTR_slip,
    MP_TYPE_FLAG_NONE,
    make_new, lwip_slip_make_new,
    locals_dict, &lwip_slip_locals_dict
    );

#endif // MICROPY_PY_LWIP_SLIP

/******************************************************************************/
// Table to convert lwIP err_t codes to socket errno codes, from the lwIP
// socket API.

// lwIP 2 changed LWIP_VERSION and it can no longer be used in macros,
// so we define our own equivalent version that can.
#define LWIP_VERSION_MACRO (LWIP_VERSION_MAJOR << 24 | LWIP_VERSION_MINOR << 16 \
        | LWIP_VERSION_REVISION << 8 | LWIP_VERSION_RC)

// Extension to lwIP error codes
#define _ERR_BADF -16
// TODO: We just know that change happened somewhere between 1.4.0 and 1.4.1,
// investigate in more detail.
#if LWIP_VERSION_MACRO < 0x01040100
static const int error_lookup_table[] = {
    0,                /* ERR_OK          0      No error, everything OK. */
    MP_ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
    MP_ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
    MP_EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
    MP_EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
    MP_EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
    MP_EINVAL,        /* ERR_VAL        -6      Illegal value.           */
    MP_EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */

    MP_ECONNABORTED,  /* ERR_ABRT       -8      Connection aborted.      */
    MP_ECONNRESET,    /* ERR_RST        -9      Connection reset.        */
    MP_ENOTCONN,      /* ERR_CLSD       -10     Connection closed.       */
    MP_ENOTCONN,      /* ERR_CONN       -11     Not connected.           */
    MP_EIO,           /* ERR_ARG        -12     Illegal argument.        */
    MP_EADDRINUSE,    /* ERR_USE        -13     Address in use.          */
    -1,               /* ERR_IF         -14     Low-level netif error    */
    MP_EALREADY,      /* ERR_ISCONN     -15     Already connected.       */
    MP_EBADF,         /* _ERR_BADF      -16     Closed socket (null pcb) */
};
#elif LWIP_VERSION_MACRO < 0x02000000
static const int error_lookup_table[] = {
    0,                /* ERR_OK          0      No error, everything OK. */
    MP_ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
    MP_ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
    MP_EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
    MP_EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
    MP_EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
    MP_EINVAL,        /* ERR_VAL        -6      Illegal value.           */
    MP_EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */

    MP_EADDRINUSE,    /* ERR_USE        -8      Address in use.          */
    MP_EALREADY,      /* ERR_ISCONN     -9      Already connected.       */
    MP_ECONNABORTED,  /* ERR_ABRT       -10     Connection aborted.      */
    MP_ECONNRESET,    /* ERR_RST        -11     Connection reset.        */
    MP_ENOTCONN,      /* ERR_CLSD       -12     Connection closed.       */
    MP_ENOTCONN,      /* ERR_CONN       -13     Not connected.           */
    MP_EIO,           /* ERR_ARG        -14     Illegal argument.        */
    -1,               /* ERR_IF         -15     Low-level netif error    */
    MP_EBADF,         /* _ERR_BADF      -16     Closed socket (null pcb) */
};
#else
// Matches lwIP 2.0.3
#undef _ERR_BADF
#define _ERR_BADF -17
static const int error_lookup_table[] = {
    0,                /* ERR_OK          0      No error, everything OK  */
    MP_ENOMEM,        /* ERR_MEM        -1      Out of memory error      */
    MP_ENOBUFS,       /* ERR_BUF        -2      Buffer error             */
    MP_EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
    MP_EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem          */
    MP_EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
    MP_EINVAL,        /* ERR_VAL        -6      Illegal value            */
    MP_EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block    */
    MP_EADDRINUSE,    /* ERR_USE        -8      Address in use           */
    MP_EALREADY,      /* ERR_ALREADY    -9      Already connecting       */
    MP_EALREADY,      /* ERR_ISCONN     -10     Conn already established */
    MP_ENOTCONN,      /* ERR_CONN       -11     Not connected            */
    -1,               /* ERR_IF         -12     Low-level netif error    */
    MP_ECONNABORTED,  /* ERR_ABRT       -13     Connection aborted       */
    MP_ECONNRESET,    /* ERR_RST        -14     Connection reset         */
    MP_ENOTCONN,      /* ERR_CLSD       -15     Connection closed        */
    MP_EIO,           /* ERR_ARG        -16     Illegal argument.        */
    MP_EBADF,         /* _ERR_BADF      -17     Closed socket (null pcb) */
};
#endif

/*******************************************************************************/
// The socket object provided by lwip.socket.

#define MOD_NETWORK_AF_INET (2)
#define MOD_NETWORK_AF_INET6 (10)

#define MOD_NETWORK_SOCK_STREAM (1)
#define MOD_NETWORK_SOCK_DGRAM (2)
#define MOD_NETWORK_SOCK_RAW (3)

typedef struct _lwip_socket_obj_t {
    mp_obj_base_t base;

    volatile union {
        struct tcp_pcb *tcp;
        struct udp_pcb *udp;
        struct raw_pcb *raw;
    } pcb;
    volatile union {
        struct pbuf *pbuf;
        struct {
            uint8_t alloc;
            uint8_t iget;
            uint8_t iput;
            union {
                struct tcp_pcb *item; // if alloc == 0
                struct tcp_pcb **array; // if alloc != 0
            } tcp;
        } connection;
    } incoming;
    mp_obj_t callback;
    byte peer[4];
    mp_uint_t peer_port;
    mp_uint_t timeout;
    uint16_t recv_offset;

    uint8_t domain;
    uint8_t type;

    #define STATE_NEW 0
    #define STATE_LISTENING 1
    #define STATE_CONNECTING 2
    #define STATE_CONNECTED 3
    #define STATE_PEER_CLOSED 4
    #define STATE_ACTIVE_UDP 5
    // Negative value is lwIP error
    int8_t state;
} lwip_socket_obj_t;

static inline bool socket_is_timedout(lwip_socket_obj_t *socket, mp_uint_t ticks_start) {
    return socket->timeout != -1 && (mp_uint_t)(mp_hal_ticks_ms() - ticks_start) >= socket->timeout;
}

static inline void poll_sockets(void) {
    mp_event_wait_ms(1);
}

static struct tcp_pcb *volatile *lwip_socket_incoming_array(lwip_socket_obj_t *socket) {
    if (socket->incoming.connection.alloc == 0) {
        return &socket->incoming.connection.tcp.item;
    } else {
        return &socket->incoming.connection.tcp.array[0];
    }
}

static void lwip_socket_free_incoming(lwip_socket_obj_t *socket) {
    bool socket_is_listener =
        socket->type == MOD_NETWORK_SOCK_STREAM
        && socket->pcb.tcp->state == LISTEN;

    if (!socket_is_listener) {
        if (socket->incoming.pbuf != NULL) {
            pbuf_free(socket->incoming.pbuf);
            socket->incoming.pbuf = NULL;
        }
    } else {
        uint8_t alloc = socket->incoming.connection.alloc;
        struct tcp_pcb *volatile *tcp_array = lwip_socket_incoming_array(socket);
        for (uint8_t i = 0; i < alloc; ++i) {
            // Deregister callback and abort
            if (tcp_array[i] != NULL) {
                tcp_poll(tcp_array[i], NULL, 0);
                tcp_abort(tcp_array[i]);
                tcp_array[i] = NULL;
            }
        }
    }
}

#if LWIP_VERSION_MAJOR < 2
#define IPADDR_STRLEN_MAX 46
#endif
mp_obj_t lwip_format_inet_addr(const ip_addr_t *ip, mp_uint_t port) {
    char ipstr[IPADDR_STRLEN_MAX];
    ipaddr_ntoa_r(ip, ipstr, sizeof(ipstr));
    mp_obj_t tuple[2] = {
        tuple[0] = mp_obj_new_str_from_cstr(ipstr),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}

mp_uint_t lwip_parse_inet_addr(mp_obj_t addr_in, ip_addr_t *out_ip) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    size_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_items[0], &addr_len);
    if (*addr_str == 0) {
        #if LWIP_IPV6
        *out_ip = *IP6_ADDR_ANY;
        #else
        *out_ip = *IP_ADDR_ANY;
        #endif
    } else {
        if (!ipaddr_aton(addr_str, out_ip)) {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
        }
    }
    return mp_obj_get_int(addr_items[1]);
}

/*******************************************************************************/
// Callback functions for the lwIP raw API.

static inline void exec_user_callback(lwip_socket_obj_t *socket) {
    if (socket->callback != MP_OBJ_NULL) {
        // Schedule the user callback to execute outside the lwIP context
        mp_sched_schedule(socket->callback, MP_OBJ_FROM_PTR(socket));
    }
}

#if MICROPY_PY_LWIP_SOCK_RAW
// Callback for incoming raw packets.
#if LWIP_VERSION_MAJOR < 2
static u8_t _lwip_raw_incoming(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
#else
static u8_t _lwip_raw_incoming(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
#endif
{
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;

    if (socket->incoming.pbuf != NULL) {
        pbuf_free(p);
    } else {
        socket->incoming.pbuf = p;
        memcpy(&socket->peer, addr, sizeof(socket->peer));
    }
    return 1; // we ate the packet
}
#endif

// Callback for incoming UDP packets. We simply stash the packet and the source address,
// in case we need it for recvfrom.
#if LWIP_VERSION_MAJOR < 2
static void _lwip_udp_incoming(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
#else
static void _lwip_udp_incoming(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
#endif
{
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;

    if (socket->incoming.pbuf != NULL) {
        // That's why they call it "unreliable". No room in the inn, drop the packet.
        pbuf_free(p);
    } else {
        socket->incoming.pbuf = p;
        socket->peer_port = (mp_uint_t)port;
        memcpy(&socket->peer, addr, sizeof(socket->peer));
    }
}

// Callback for general tcp errors.
static void _lwip_tcp_error(void *arg, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;

    // Free any incoming buffers or connections that are stored
    lwip_socket_free_incoming(socket);
    // Pass the error code back via the connection variable.
    socket->state = err;
    // If we got here, the lwIP stack either has deallocated or will deallocate the pcb.
    socket->pcb.tcp = NULL;
}

// Callback for tcp connection requests. Error code err is unused. (See tcp.h)
static err_t _lwip_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;

    socket->state = STATE_CONNECTED;
    return ERR_OK;
}

// Handle errors (eg connection aborted) on TCP PCBs that have been put on the
// accept queue but are not yet actually accepted.
static void _lwip_tcp_err_unaccepted(void *arg, err_t err) {
    struct tcp_pcb *pcb = (struct tcp_pcb *)arg;

    // The ->connected entry is repurposed to store the parent socket; this is safe
    // because it's only ever used by lwIP if tcp_connect is called on the TCP PCB.
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)pcb->connected;

    // Array is not volatile because thiss callback is executed within the lwIP context
    uint8_t alloc = socket->incoming.connection.alloc;
    struct tcp_pcb **tcp_array = (struct tcp_pcb **)lwip_socket_incoming_array(socket);

    // Search for PCB on the accept queue of the parent socket
    struct tcp_pcb **shift_down = NULL;
    uint8_t i = socket->incoming.connection.iget;
    do {
        if (shift_down == NULL) {
            if (tcp_array[i] == pcb) {
                shift_down = &tcp_array[i];
            }
        } else {
            *shift_down = tcp_array[i];
            shift_down = &tcp_array[i];
        }
        if (++i >= alloc) {
            i = 0;
        }
    } while (i != socket->incoming.connection.iput);

    // PCB found in queue, remove it
    if (shift_down != NULL) {
        *shift_down = NULL;
        socket->incoming.connection.iput = shift_down - tcp_array;
    }
}

// By default, a child socket of listen socket is created with recv
// handler which discards incoming pbuf's. We don't want to do that,
// so set this handler which requests lwIP to keep pbuf's and deliver
// them later. We cannot cache pbufs in child socket on Python side,
// until it is created in accept().
static err_t _lwip_tcp_recv_unaccepted(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    return ERR_BUF;
}

// Callback for incoming tcp connections.
static err_t _lwip_tcp_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // err can be ERR_MEM to notify us that there was no memory for an incoming connection
    if (err != ERR_OK) {
        return ERR_OK;
    }

    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;
    tcp_recv(newpcb, _lwip_tcp_recv_unaccepted);

    // Search for an empty slot to store the new connection
    struct tcp_pcb *volatile *slot = &lwip_socket_incoming_array(socket)[socket->incoming.connection.iput];
    if (*slot == NULL) {
        // Have an empty slot to store waiting connection
        *slot = newpcb;
        if (++socket->incoming.connection.iput >= socket->incoming.connection.alloc) {
            socket->incoming.connection.iput = 0;
        }

        // Schedule user accept callback
        exec_user_callback(socket);

        // Set the error callback to handle the case of a dropped connection before we
        // have a chance to take it off the accept queue.
        // The ->connected entry is repurposed to store the parent socket; this is safe
        // because it's only ever used by lwIP if tcp_connect is called on the TCP PCB.
        newpcb->connected = (void *)socket;
        tcp_arg(newpcb, newpcb);
        tcp_err(newpcb, _lwip_tcp_err_unaccepted);

        return ERR_OK;
    }

    DEBUG_printf("_lwip_tcp_accept: No room to queue pcb waiting for accept\n");
    return ERR_BUF;
}

// Callback for inbound tcp packets.
static err_t _lwip_tcp_recv(void *arg, struct tcp_pcb *tcpb, struct pbuf *p, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t *)arg;

    if (p == NULL) {
        // Other side has closed connection.
        DEBUG_printf("_lwip_tcp_recv[%p]: other side closed connection\n", socket);
        socket->state = STATE_PEER_CLOSED;
        exec_user_callback(socket);
        return ERR_OK;
    }

    if (socket->incoming.pbuf == NULL) {
        socket->incoming.pbuf = p;
    } else {
        #ifdef SOCKET_SINGLE_PBUF
        return ERR_BUF;
        #else
        pbuf_cat(socket->incoming.pbuf, p);
        #endif
    }

    exec_user_callback(socket);

    return ERR_OK;
}

/*******************************************************************************/
// Functions for socket send/receive operations. Socket send/recv and friends call
// these to do the work.

// Helper function for send/sendto to handle raw/UDP packets.
static mp_uint_t lwip_raw_udp_send(lwip_socket_obj_t *socket, const byte *buf, mp_uint_t len, ip_addr_t *ip, mp_uint_t port, int *_errno) {
    if (len > 0xffff) {
        // Any packet that big is probably going to fail the pbuf_alloc anyway, but may as well try
        len = 0xffff;
    }

    MICROPY_PY_LWIP_ENTER

    // FIXME: maybe PBUF_ROM?
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        MICROPY_PY_LWIP_EXIT
        *_errno = MP_ENOMEM;
        return -1;
    }

    memcpy(p->payload, buf, len);

    err_t err;
    if (ip == NULL) {
        #if MICROPY_PY_LWIP_SOCK_RAW
        if (socket->type == MOD_NETWORK_SOCK_RAW) {
            err = raw_send(socket->pcb.raw, p);
        } else
        #endif
        {
            err = udp_send(socket->pcb.udp, p);
        }
    } else {
        #if MICROPY_PY_LWIP_SOCK_RAW
        if (socket->type == MOD_NETWORK_SOCK_RAW) {
            err = raw_sendto(socket->pcb.raw, p, ip);
        } else
        #endif
        {
            err = udp_sendto(socket->pcb.udp, p, ip, port);
        }
    }

    pbuf_free(p);

    MICROPY_PY_LWIP_EXIT

    // udp_sendto can return 1 on occasion for ESP8266 port.  It's not known why
    // but it seems that the send actually goes through without error in this case.
    // So we treat such cases as a success until further investigation.
    if (err != ERR_OK && err != 1) {
        *_errno = error_lookup_table[-err];
        return -1;
    }

    return len;
}

// Helper function for recv/recvfrom to handle raw/UDP packets
static mp_uint_t lwip_raw_udp_receive(lwip_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {

    if (socket->incoming.pbuf == NULL) {
        if (socket->timeout == 0) {
            // Non-blocking socket.
            *_errno = MP_EAGAIN;
            return -1;
        }

        // Wait for data to arrive on UDP socket.
        mp_uint_t start = mp_hal_ticks_ms();
        while (socket->incoming.pbuf == NULL) {
            if (socket->timeout != -1 && mp_hal_ticks_ms() - start > socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            poll_sockets();
        }
    }

    if (ip != NULL) {
        memcpy(ip, &socket->peer, sizeof(socket->peer));
        *port = socket->peer_port;
    }

    struct pbuf *p = socket->incoming.pbuf;

    MICROPY_PY_LWIP_ENTER

    u16_t result = pbuf_copy_partial(p, buf, ((p->tot_len > len) ? len : p->tot_len), 0);
    pbuf_free(p);
    socket->incoming.pbuf = NULL;

    MICROPY_PY_LWIP_EXIT

    return (mp_uint_t)result;
}

// For use in stream virtual methods
#define STREAM_ERROR_CHECK(socket) \
    if (socket->state < 0) { \
        *_errno = error_lookup_table[-socket->state]; \
        return MP_STREAM_ERROR; \
    } \
    assert(socket->pcb.tcp);

// Version of above for use when lock is held
#define STREAM_ERROR_CHECK_WITH_LOCK(socket) \
    if (socket->state < 0) { \
        *_errno = error_lookup_table[-socket->state]; \
        MICROPY_PY_LWIP_EXIT \
        return MP_STREAM_ERROR; \
    } \
    assert(socket->pcb.tcp);


// Helper function for send/sendto to handle TCP packets
static mp_uint_t lwip_tcp_send(lwip_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    // Check for any pending errors
    STREAM_ERROR_CHECK(socket);

    MICROPY_PY_LWIP_ENTER

    // If the socket is still connecting then don't let data be written to it.
    // Otherwise, get the number of available bytes in the output buffer.
    u16_t available = 0;
    if (socket->state != STATE_CONNECTING) {
        available = tcp_sndbuf(socket->pcb.tcp);
    }

    if (available == 0) {
        // Non-blocking socket
        if (socket->timeout == 0) {
            MICROPY_PY_LWIP_EXIT
            *_errno = MP_EAGAIN;
            return MP_STREAM_ERROR;
        }

        mp_uint_t start = mp_hal_ticks_ms();
        // Assume that STATE_PEER_CLOSED may mean half-closed connection, where peer closed it
        // sending direction, but not receiving. Consequently, check for both STATE_CONNECTED
        // and STATE_PEER_CLOSED as normal conditions and still waiting for buffers to be sent.
        // If peer fully closed socket, we would have socket->state set to ERR_RST (connection
        // reset) by error callback.
        // Avoid sending too small packets, so wait until at least 16 bytes available
        while (socket->state == STATE_CONNECTING
               || (socket->state >= STATE_CONNECTED && (available = tcp_sndbuf(socket->pcb.tcp)) < 16)) {
            MICROPY_PY_LWIP_EXIT
            if (socket->timeout != -1 && mp_hal_ticks_ms() - start > socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return MP_STREAM_ERROR;
            }
            poll_sockets();
            MICROPY_PY_LWIP_REENTER
        }

        // While we waited, something could happen
        STREAM_ERROR_CHECK_WITH_LOCK(socket);
    }

    u16_t write_len = MIN(available, len);

    // If tcp_write returns ERR_MEM then there's currently not enough memory to
    // queue the write, so wait and keep trying until it succeeds (with 10s limit).
    // Note: if the socket is non-blocking then this code will actually block until
    // there's enough memory to do the write, but by this stage we have already
    // committed to being able to write the data.
    err_t err;
    for (int i = 0; i < 200; ++i) {
        err = tcp_write(socket->pcb.tcp, buf, write_len, TCP_WRITE_FLAG_COPY);
        if (err != ERR_MEM) {
            break;
        }
        err = tcp_output(socket->pcb.tcp);
        if (err != ERR_OK) {
            break;
        }
        MICROPY_PY_LWIP_EXIT
        mp_hal_delay_ms(50);
        MICROPY_PY_LWIP_REENTER
    }

    // Use nagle algorithm to determine when to send segment buffer (can be
    // disabled with TCP_NODELAY socket option)
    if (err == ERR_OK) {
        err = tcp_output_nagle(socket->pcb.tcp);
    }

    MICROPY_PY_LWIP_EXIT

    if (err != ERR_OK) {
        *_errno = error_lookup_table[-err];
        return MP_STREAM_ERROR;
    }

    return write_len;
}

// Helper function for recv/recvfrom to handle TCP packets
static mp_uint_t lwip_tcp_receive(lwip_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    // Check for any pending errors
    STREAM_ERROR_CHECK(socket);

    if (socket->incoming.pbuf == NULL) {

        // Non-blocking socket
        if (socket->timeout == 0) {
            if (socket->state == STATE_PEER_CLOSED) {
                return 0;
            }
            *_errno = MP_EAGAIN;
            return -1;
        }

        mp_uint_t start = mp_hal_ticks_ms();
        while (socket->state == STATE_CONNECTED && socket->incoming.pbuf == NULL) {
            if (socket->timeout != -1 && mp_hal_ticks_ms() - start > socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            poll_sockets();
        }

        if (socket->state == STATE_PEER_CLOSED) {
            if (socket->incoming.pbuf == NULL) {
                // socket closed and no data left in buffer
                return 0;
            }
        } else if (socket->state != STATE_CONNECTED) {
            if (socket->state >= STATE_NEW) {
                *_errno = MP_ENOTCONN;
            } else {
                *_errno = error_lookup_table[-socket->state];
            }
            return -1;
        }
    }

    MICROPY_PY_LWIP_ENTER

    assert(socket->pcb.tcp != NULL);

    struct pbuf *p = socket->incoming.pbuf;

    mp_uint_t remaining = p->len - socket->recv_offset;
    if (len > remaining) {
        len = remaining;
    }

    memcpy(buf, (byte *)p->payload + socket->recv_offset, len);

    remaining -= len;
    if (remaining == 0) {
        socket->incoming.pbuf = p->next;
        // If we don't ref here, free() will free the entire chain,
        // if we ref, it does what we need: frees 1st buf, and decrements
        // next buf's refcount back to 1.
        pbuf_ref(p->next);
        pbuf_free(p);
        socket->recv_offset = 0;
    } else {
        socket->recv_offset += len;
    }
    tcp_recved(socket->pcb.tcp, len);

    MICROPY_PY_LWIP_EXIT

    return len;
}

/*******************************************************************************/
// The socket functions provided by lwip.socket.

static const mp_obj_type_t lwip_socket_type;

static void lwip_socket_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    lwip_socket_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "<socket state=%d timeout=%d incoming=%p off=%d>", self->state, self->timeout,
        self->incoming.pbuf, self->recv_offset);
}

// FIXME: Only supports two arguments at present
static mp_obj_t lwip_socket_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    lwip_socket_obj_t *socket = mp_obj_malloc_with_finaliser(lwip_socket_obj_t, &lwip_socket_type);
    socket->timeout = -1;
    socket->recv_offset = 0;
    socket->domain = MOD_NETWORK_AF_INET;
    socket->type = MOD_NETWORK_SOCK_STREAM;
    socket->callback = MP_OBJ_NULL;
    socket->state = STATE_NEW;

    if (n_args >= 1) {
        socket->domain = mp_obj_get_int(args[0]);
        if (n_args >= 2) {
            socket->type = mp_obj_get_int(args[1]);
        }
    }

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM:
            socket->pcb.tcp = tcp_new();
            socket->incoming.connection.alloc = 0;
            socket->incoming.connection.tcp.item = NULL;
            break;
        case MOD_NETWORK_SOCK_DGRAM:
            socket->pcb.udp = udp_new();
            socket->incoming.pbuf = NULL;
            break;
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW: {
            mp_int_t proto = n_args <= 2 ? 0 : mp_obj_get_int(args[2]);
            socket->pcb.raw = raw_new(proto);
            break;
        }
        #endif
        default:
            mp_raise_OSError(MP_EINVAL);
    }

    if (socket->pcb.tcp == NULL) {
        mp_raise_OSError(MP_ENOMEM);
    }

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            // Register the socket object as our callback argument.
            tcp_arg(socket->pcb.tcp, (void *)socket);
            // Register our error callback.
            tcp_err(socket->pcb.tcp, _lwip_tcp_error);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            socket->state = STATE_ACTIVE_UDP;
            // Register our receive callback now. Since UDP sockets don't require binding or connection
            // before use, there's no other good time to do it.
            udp_recv(socket->pcb.udp, _lwip_udp_incoming, (void *)socket);
            break;
        }
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW: {
            // Register our receive callback now. Since raw sockets don't require binding or connection
            // before use, there's no other good time to do it.
            raw_recv(socket->pcb.raw, _lwip_raw_incoming, (void *)socket);
            break;
        }
        #endif
    }

    return MP_OBJ_FROM_PTR(socket);
}

static mp_obj_t lwip_socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);

    ip_addr_t bind_addr;
    mp_uint_t port = lwip_parse_inet_addr(addr_in, &bind_addr);

    err_t err = ERR_ARG;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            err = tcp_bind(socket->pcb.tcp, &bind_addr, port);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            err = udp_bind(socket->pcb.udp, &bind_addr, port);
            break;
        }
    }

    if (err != ERR_OK) {
        mp_raise_OSError(error_lookup_table[-err]);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_bind_obj, lwip_socket_bind);

static mp_obj_t lwip_socket_listen(size_t n_args, const mp_obj_t *args) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(args[0]);

    mp_int_t backlog = MICROPY_PY_SOCKET_LISTEN_BACKLOG_DEFAULT;
    if (n_args > 1) {
        backlog = mp_obj_get_int(args[1]);
        backlog = (backlog < 0) ? 0 : backlog;
    }

    if (socket->pcb.tcp == NULL) {
        mp_raise_OSError(MP_EBADF);
    }
    if (socket->type != MOD_NETWORK_SOCK_STREAM) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }
    #if LWIP_IPV6
    if (ip_addr_cmp(&socket->pcb.tcp->local_ip, IP6_ADDR_ANY)) {
        IP_SET_TYPE_VAL(socket->pcb.tcp->local_ip,  IPADDR_TYPE_ANY);
        IP_SET_TYPE_VAL(socket->pcb.tcp->remote_ip, IPADDR_TYPE_ANY);
    }
    #endif

    struct tcp_pcb *new_pcb;
    #if LWIP_VERSION_MACRO < 0x02000100
    new_pcb = tcp_listen_with_backlog(socket->pcb.tcp, (u8_t)backlog);
    #else
    err_t error;
    new_pcb = tcp_listen_with_backlog_and_err(socket->pcb.tcp, (u8_t)backlog, &error);
    #endif

    if (new_pcb == NULL) {
        #if LWIP_VERSION_MACRO < 0x02000100
        mp_raise_OSError(MP_ENOMEM);
        #else
        mp_raise_OSError(error_lookup_table[-error]);
        #endif
    }
    socket->pcb.tcp = new_pcb;

    // Allocate memory for the backlog of connections
    if (backlog <= 1) {
        socket->incoming.connection.alloc = 0;
        socket->incoming.connection.tcp.item = NULL;
    } else {
        socket->incoming.connection.alloc = backlog;
        socket->incoming.connection.tcp.array = m_new0(struct tcp_pcb *, backlog);
    }
    socket->incoming.connection.iget = 0;
    socket->incoming.connection.iput = 0;

    tcp_accept(new_pcb, _lwip_tcp_accept);

    // Socket is no longer considered "new" for purposes of polling
    socket->state = STATE_LISTENING;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwip_socket_listen_obj, 1, 2, lwip_socket_listen);

static mp_obj_t lwip_socket_accept(mp_obj_t self_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);

    if (socket->type != MOD_NETWORK_SOCK_STREAM) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }

    // Create new socket object, do it here because we must not raise an out-of-memory
    // exception when the LWIP concurrency lock is held
    lwip_socket_obj_t *socket2 = mp_obj_malloc_with_finaliser(lwip_socket_obj_t, &lwip_socket_type);

    MICROPY_PY_LWIP_ENTER

    if (socket->pcb.tcp == NULL) {
        MICROPY_PY_LWIP_EXIT
        m_del_obj(lwip_socket_obj_t, socket2);
        mp_raise_OSError(MP_EBADF);
    }

    // I need to do this because "tcp_accepted", later, is a macro.
    struct tcp_pcb *listener = socket->pcb.tcp;
    if (listener->state != LISTEN) {
        MICROPY_PY_LWIP_EXIT
        m_del_obj(lwip_socket_obj_t, socket2);
        mp_raise_OSError(MP_EINVAL);
    }

    // accept incoming connection
    struct tcp_pcb *volatile *incoming_connection = &lwip_socket_incoming_array(socket)[socket->incoming.connection.iget];
    if (*incoming_connection == NULL) {
        mp_uint_t ticks_start = mp_hal_ticks_ms();
        for (;;) {
            MICROPY_PY_LWIP_EXIT
            poll_sockets();
            MICROPY_PY_LWIP_REENTER
            if (*incoming_connection != NULL) {
                break;
            }
            if (socket_is_timedout(socket, ticks_start)) {
                MICROPY_PY_LWIP_EXIT
                m_del_obj(lwip_socket_obj_t, socket2);
                if (socket->timeout == 0) {
                    mp_raise_OSError(MP_EAGAIN);
                } else {
                    mp_raise_OSError(MP_ETIMEDOUT);
                }
            }
        }
    }

    // We get a new pcb handle...
    socket2->pcb.tcp = *incoming_connection;
    if (++socket->incoming.connection.iget >= socket->incoming.connection.alloc) {
        socket->incoming.connection.iget = 0;
    }
    *incoming_connection = NULL;

    // ...and set up the new socket for it.
    socket2->domain = MOD_NETWORK_AF_INET;
    socket2->type = MOD_NETWORK_SOCK_STREAM;
    socket2->incoming.pbuf = NULL;
    socket2->timeout = socket->timeout;
    socket2->state = STATE_CONNECTED;
    socket2->recv_offset = 0;
    socket2->callback = MP_OBJ_NULL;
    tcp_arg(socket2->pcb.tcp, (void *)socket2);
    tcp_err(socket2->pcb.tcp, _lwip_tcp_error);
    tcp_recv(socket2->pcb.tcp, _lwip_tcp_recv);

    tcp_accepted(listener);

    MICROPY_PY_LWIP_EXIT

    // make the return value
    mp_uint_t port = (mp_uint_t)socket2->pcb.tcp->remote_port;
    mp_obj_tuple_t *client = MP_OBJ_TO_PTR(mp_obj_new_tuple(2, NULL));
    client->items[0] = MP_OBJ_FROM_PTR(socket2);
    client->items[1] = lwip_format_inet_addr(&socket2->pcb.tcp->remote_ip, port);

    return MP_OBJ_FROM_PTR(client);
}
static MP_DEFINE_CONST_FUN_OBJ_1(lwip_socket_accept_obj, lwip_socket_accept);

static mp_obj_t lwip_socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);

    if (socket->pcb.tcp == NULL) {
        mp_raise_OSError(MP_EBADF);
    }

    // get address
    ip_addr_t dest;
    mp_uint_t port = lwip_parse_inet_addr(addr_in, &dest);

    err_t err = ERR_ARG;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            if (socket->state != STATE_NEW) {
                if (socket->state == STATE_CONNECTED) {
                    mp_raise_OSError(MP_EISCONN);
                } else {
                    mp_raise_OSError(MP_EALREADY);
                }
            }

            // Register our receive callback.
            MICROPY_PY_LWIP_ENTER
            tcp_recv(socket->pcb.tcp, _lwip_tcp_recv);
            socket->state = STATE_CONNECTING;
            err = tcp_connect(socket->pcb.tcp, &dest, port, _lwip_tcp_connected);
            if (err != ERR_OK) {
                MICROPY_PY_LWIP_EXIT
                socket->state = STATE_NEW;
                mp_raise_OSError(error_lookup_table[-err]);
            }
            socket->peer_port = (mp_uint_t)port;
            memcpy(socket->peer, &dest, sizeof(socket->peer));
            MICROPY_PY_LWIP_EXIT

            // And now we wait...
            mp_uint_t ticks_start = mp_hal_ticks_ms();
            for (;;) {
                poll_sockets();
                if (socket->state != STATE_CONNECTING) {
                    break;
                }
                if (socket_is_timedout(socket, ticks_start)) {
                    if (socket->timeout == 0) {
                        mp_raise_OSError(MP_EINPROGRESS);
                    } else {
                        mp_raise_OSError(MP_ETIMEDOUT);
                    }
                }
            }

            if (socket->state == STATE_CONNECTED) {
                err = ERR_OK;
            } else {
                err = socket->state;
            }
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            err = udp_connect(socket->pcb.udp, &dest, port);
            break;
        }
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW: {
            err = raw_connect(socket->pcb.raw, &dest);
            break;
        }
        #endif
    }

    if (err != ERR_OK) {
        mp_raise_OSError(error_lookup_table[-err]);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_connect_obj, lwip_socket_connect);

static void lwip_socket_check_connected(lwip_socket_obj_t *socket) {
    if (socket->pcb.tcp == NULL) {
        // not connected
        int _errno = error_lookup_table[-socket->state];
        socket->state = _ERR_BADF;
        mp_raise_OSError(_errno);
    }
}

static mp_obj_t lwip_socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    int _errno;

    lwip_socket_check_connected(socket);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, bufinfo.buf, bufinfo.len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_send(socket, bufinfo.buf, bufinfo.len, NULL, 0, &_errno);
            break;
    }
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }

    return mp_obj_new_int_from_uint(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_send_obj, lwip_socket_send);

static mp_obj_t lwip_socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    int _errno;

    lwip_socket_check_connected(socket);

    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_receive(socket, (byte *)vstr.buf, len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_receive(socket, (byte *)vstr.buf, len, NULL, NULL, &_errno);
            break;
    }
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }

    if (ret == 0) {
        return mp_const_empty_bytes;
    }
    vstr.len = ret;
    return mp_obj_new_bytes_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_recv_obj, lwip_socket_recv);

static mp_obj_t lwip_socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    int _errno;

    lwip_socket_check_connected(socket);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    ip_addr_t ip;
    mp_uint_t port = lwip_parse_inet_addr(addr_in, &ip);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, bufinfo.buf, bufinfo.len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_send(socket, bufinfo.buf, bufinfo.len, &ip, port, &_errno);
            break;
    }
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }

    return mp_obj_new_int_from_uint(ret);
}
static MP_DEFINE_CONST_FUN_OBJ_3(lwip_socket_sendto_obj, lwip_socket_sendto);

static mp_obj_t lwip_socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    int _errno;

    lwip_socket_check_connected(socket);

    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);
    byte ip[4];
    mp_uint_t port;

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            memcpy(ip, &socket->peer, sizeof(socket->peer));
            port = (mp_uint_t)socket->peer_port;
            ret = lwip_tcp_receive(socket, (byte *)vstr.buf, len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_receive(socket, (byte *)vstr.buf, len, ip, &port, &_errno);
            break;
    }
    if (ret == -1) {
        mp_raise_OSError(_errno);
    }

    mp_obj_t tuple[2];
    if (ret == 0) {
        tuple[0] = mp_const_empty_bytes;
    } else {
        vstr.len = ret;
        tuple[0] = mp_obj_new_bytes_from_vstr(&vstr);
    }
    tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);
    return mp_obj_new_tuple(2, tuple);
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_recvfrom_obj, lwip_socket_recvfrom);

static mp_obj_t lwip_socket_sendall(mp_obj_t self_in, mp_obj_t buf_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    lwip_socket_check_connected(socket);

    int _errno;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            if (socket->timeout == 0) {
                // Behavior of sendall() for non-blocking sockets isn't explicitly specified.
                // But it's specified that "On error, an exception is raised, there is no
                // way to determine how much data, if any, was successfully sent." Then, the
                // most useful behavior is: check whether we will be able to send all of input
                // data without EAGAIN, and if won't be, raise it without sending any.
                if (bufinfo.len > tcp_sndbuf(socket->pcb.tcp)) {
                    mp_raise_OSError(MP_EAGAIN);
                }
            }
            // TODO: In CPython3.5, socket timeout should apply to the
            // entire sendall() operation, not to individual send() chunks.
            while (bufinfo.len != 0) {
                ret = lwip_tcp_send(socket, bufinfo.buf, bufinfo.len, &_errno);
                if (ret == -1) {
                    mp_raise_OSError(_errno);
                }
                bufinfo.len -= ret;
                bufinfo.buf = (char *)bufinfo.buf + ret;
            }
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM:
            mp_raise_NotImplementedError(NULL);
            break;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_sendall_obj, lwip_socket_sendall);

static mp_obj_t lwip_socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    mp_uint_t timeout;
    if (timeout_in == mp_const_none) {
        timeout = -1;
    } else {
        #if MICROPY_PY_BUILTINS_FLOAT
        timeout = (mp_uint_t)(MICROPY_FLOAT_CONST(1000.0) * mp_obj_get_float(timeout_in));
        #else
        timeout = 1000 * mp_obj_get_int(timeout_in);
        #endif
    }
    socket->timeout = timeout;
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_settimeout_obj, lwip_socket_settimeout);

static mp_obj_t lwip_socket_setblocking(mp_obj_t self_in, mp_obj_t flag_in) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    bool val = mp_obj_is_true(flag_in);
    if (val) {
        socket->timeout = -1;
    } else {
        socket->timeout = 0;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_setblocking_obj, lwip_socket_setblocking);

#if LWIP_VERSION_MAJOR < 2
#define MP_IGMP_IP_ADDR_TYPE ip_addr_t
#else
#define MP_IGMP_IP_ADDR_TYPE ip4_addr_t
#endif

static mp_obj_t lwip_socket_setsockopt(size_t n_args, const mp_obj_t *args) {
    (void)n_args; // always 4
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(args[0]);

    int opt = mp_obj_get_int(args[2]);
    if (opt == 20) {
        if (args[3] == mp_const_none) {
            socket->callback = MP_OBJ_NULL;
        } else {
            socket->callback = args[3];
        }
        return mp_const_none;
    }

    switch (opt) {
        // level: SOL_SOCKET
        case SOF_REUSEADDR:
        case SOF_BROADCAST: {
            mp_int_t val = mp_obj_get_int(args[3]);
            // Options are common for UDP and TCP pcb's.
            if (val) {
                ip_set_option(socket->pcb.tcp, opt);
            } else {
                ip_reset_option(socket->pcb.tcp, opt);
            }
            break;
        }

        // level: IPPROTO_IP
        case IP_ADD_MEMBERSHIP:
        case IP_DROP_MEMBERSHIP: {
            mp_buffer_info_t bufinfo;
            mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
            if (bufinfo.len != sizeof(MP_IGMP_IP_ADDR_TYPE) * 2) {
                mp_raise_ValueError(NULL);
            }

            // POSIX setsockopt has order: group addr, if addr, lwIP has it vice-versa
            err_t err;
            if (opt == IP_ADD_MEMBERSHIP) {
                err = igmp_joingroup((MP_IGMP_IP_ADDR_TYPE *)bufinfo.buf + 1, bufinfo.buf);
            } else {
                err = igmp_leavegroup((MP_IGMP_IP_ADDR_TYPE *)bufinfo.buf + 1, bufinfo.buf);
            }
            if (err != ERR_OK) {
                mp_raise_OSError(error_lookup_table[-err]);
            }
            break;
        }

        // level: IPPROTO_TCP
        case TCP_NODELAY: {
            mp_int_t val = mp_obj_get_int(args[3]);
            if (val) {
                tcp_set_flags(socket->pcb.tcp, opt);
            } else {
                tcp_clear_flags(socket->pcb.tcp, opt);
            }
            break;
        }

        default:
            printf("Warning: lwip.setsockopt() not implemented\n");
    }
    return mp_const_none;
}

#undef MP_IGMP_IP_ADDR_TYPE

static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwip_socket_setsockopt_obj, 4, 4, lwip_socket_setsockopt);

static mp_obj_t lwip_socket_makefile(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    return args[0];
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwip_socket_makefile_obj, 1, 3, lwip_socket_makefile);

static mp_uint_t lwip_socket_read(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM:
            return lwip_tcp_receive(socket, buf, size, errcode);
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            return lwip_raw_udp_receive(socket, buf, size, NULL, NULL, errcode);
    }
    // Unreachable
    return MP_STREAM_ERROR;
}

static mp_uint_t lwip_socket_write(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM:
            return lwip_tcp_send(socket, buf, size, errcode);
        case MOD_NETWORK_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case MOD_NETWORK_SOCK_RAW:
        #endif
            return lwip_raw_udp_send(socket, buf, size, NULL, 0, errcode);
    }
    // Unreachable
    return MP_STREAM_ERROR;
}

static err_t _lwip_tcp_close_poll(void *arg, struct tcp_pcb *pcb) {
    // Connection has not been cleanly closed so just abort it to free up memory
    tcp_poll(pcb, NULL, 0);
    tcp_abort(pcb);
    return ERR_OK;
}

static mp_uint_t lwip_socket_ioctl(mp_obj_t self_in, mp_uint_t request, uintptr_t arg, int *errcode) {
    lwip_socket_obj_t *socket = MP_OBJ_TO_PTR(self_in);
    mp_uint_t ret;

    MICROPY_PY_LWIP_ENTER

    if (request == MP_STREAM_POLL) {
        uintptr_t flags = arg;
        ret = 0;

        if (flags & MP_STREAM_POLL_RD) {
            if (socket->state == STATE_LISTENING) {
                // Listening TCP socket may have one or multiple connections waiting
                if (lwip_socket_incoming_array(socket)[socket->incoming.connection.iget] != NULL) {
                    ret |= MP_STREAM_POLL_RD;
                }
            } else {
                // Otherwise there is just one slot for incoming data
                if (socket->incoming.pbuf != NULL) {
                    ret |= MP_STREAM_POLL_RD;
                }
            }
        }

        if (flags & MP_STREAM_POLL_WR) {
            if (socket->type == MOD_NETWORK_SOCK_DGRAM && socket->pcb.udp != NULL) {
                // UDP socket is writable
                ret |= MP_STREAM_POLL_WR;
            #if MICROPY_PY_LWIP_SOCK_RAW
            } else if (socket->type == MOD_NETWORK_SOCK_RAW && socket->pcb.raw != NULL) {
                // raw socket is writable
                ret |= MP_STREAM_POLL_WR;
            #endif
            } else if (socket->state != STATE_CONNECTING && socket->pcb.tcp != NULL && tcp_sndbuf(socket->pcb.tcp) > 0) {
                // TCP socket is writable
                // Note: pcb.tcp==NULL if state<0, and in this case we can't call tcp_sndbuf
                ret |= MP_STREAM_POLL_WR;
            }
        }

        if (socket->state == STATE_NEW) {
            // New sockets are not connected so set HUP
            ret |= MP_STREAM_POLL_HUP;
        } else if (socket->state == STATE_PEER_CLOSED) {
            // Peer-closed socket is both readable and writable: read will
            // return EOF, write - error. Without this poll will hang on a
            // socket which was closed by peer.
            ret |= flags & (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR);
        } else if (socket->state == ERR_RST) {
            // Socket was reset by peer, a write will return an error
            ret |= flags & MP_STREAM_POLL_WR;
            ret |= MP_STREAM_POLL_HUP;
        } else if (socket->state == _ERR_BADF) {
            ret |= MP_STREAM_POLL_NVAL;
        } else if (socket->state < 0) {
            // Socket in some other error state, use catch-all ERR flag
            // TODO: may need to set other return flags here
            ret |= MP_STREAM_POLL_ERR;
        }

    } else if (request == MP_STREAM_CLOSE) {
        if (socket->pcb.tcp == NULL) {
            MICROPY_PY_LWIP_EXIT
            return 0;
        }

        // Free any incoming buffers or connections that are stored
        lwip_socket_free_incoming(socket);

        switch (socket->type) {
            case MOD_NETWORK_SOCK_STREAM: {
                // Deregister callback (pcb.tcp is set to NULL below so must deregister now)
                tcp_arg(socket->pcb.tcp, NULL);
                tcp_err(socket->pcb.tcp, NULL);
                tcp_recv(socket->pcb.tcp, NULL);

                if (socket->pcb.tcp->state != LISTEN) {
                    // Schedule a callback to abort the connection if it's not cleanly closed after
                    // the given timeout.  The callback must be set before calling tcp_close since
                    // the latter may free the pcb; if it doesn't then the callback will be active.
                    tcp_poll(socket->pcb.tcp, _lwip_tcp_close_poll, MICROPY_PY_LWIP_TCP_CLOSE_TIMEOUT_MS / 500);
                }
                if (tcp_close(socket->pcb.tcp) != ERR_OK) {
                    DEBUG_printf("lwip_close: had to call tcp_abort()\n");
                    tcp_abort(socket->pcb.tcp);
                }
                break;
            }
            case MOD_NETWORK_SOCK_DGRAM:
                udp_recv(socket->pcb.udp, NULL, NULL);
                udp_remove(socket->pcb.udp);
                break;
            #if MICROPY_PY_LWIP_SOCK_RAW
            case MOD_NETWORK_SOCK_RAW:
                raw_recv(socket->pcb.raw, NULL, NULL);
                raw_remove(socket->pcb.raw);
                break;
            #endif
        }

        socket->pcb.tcp = NULL;
        socket->state = _ERR_BADF;
        ret = 0;

    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }

    MICROPY_PY_LWIP_EXIT

    return ret;
}

static const mp_rom_map_elem_t lwip_socket_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&mp_stream_close_obj) },
    { MP_ROM_QSTR(MP_QSTR_bind), MP_ROM_PTR(&lwip_socket_bind_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&lwip_socket_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_accept), MP_ROM_PTR(&lwip_socket_accept_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&lwip_socket_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&lwip_socket_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_recv), MP_ROM_PTR(&lwip_socket_recv_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendto), MP_ROM_PTR(&lwip_socket_sendto_obj) },
    { MP_ROM_QSTR(MP_QSTR_recvfrom), MP_ROM_PTR(&lwip_socket_recvfrom_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendall), MP_ROM_PTR(&lwip_socket_sendall_obj) },
    { MP_ROM_QSTR(MP_QSTR_settimeout), MP_ROM_PTR(&lwip_socket_settimeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_setblocking), MP_ROM_PTR(&lwip_socket_setblocking_obj) },
    { MP_ROM_QSTR(MP_QSTR_setsockopt), MP_ROM_PTR(&lwip_socket_setsockopt_obj) },
    { MP_ROM_QSTR(MP_QSTR_makefile), MP_ROM_PTR(&lwip_socket_makefile_obj) },

    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_stream_write_obj) },
};
static MP_DEFINE_CONST_DICT(lwip_socket_locals_dict, lwip_socket_locals_dict_table);

static const mp_stream_p_t lwip_socket_stream_p = {
    .read = lwip_socket_read,
    .write = lwip_socket_write,
    .ioctl = lwip_socket_ioctl,
};

static MP_DEFINE_CONST_OBJ_TYPE(
    lwip_socket_type,
    MP_QSTR_socket,
    MP_TYPE_FLAG_NONE,
    make_new, lwip_socket_make_new,
    print, lwip_socket_print,
    protocol, &lwip_socket_stream_p,
    locals_dict, &lwip_socket_locals_dict
    );

/******************************************************************************/
// Support functions for memory protection. lwIP has its own memory management
// routines for its internal structures, and since they might be called in
// interrupt handlers, they need some protection.
sys_prot_t sys_arch_protect() {
    return (sys_prot_t)MICROPY_BEGIN_ATOMIC_SECTION();
}

void sys_arch_unprotect(sys_prot_t state) {
    MICROPY_END_ATOMIC_SECTION((mp_uint_t)state);
}

/******************************************************************************/
// Polling callbacks for the interfaces connected to lwIP. Right now it calls
// itself a "list" but isn't; we only support a single interface.

typedef struct nic_poll {
    void (*poll)(void *arg);
    void *poll_arg;
} nic_poll_t;

static nic_poll_t lwip_poll_list;

void mod_lwip_register_poll(void (*poll)(void *arg), void *poll_arg) {
    lwip_poll_list.poll = poll;
    lwip_poll_list.poll_arg = poll_arg;
}

void mod_lwip_deregister_poll(void (*poll)(void *arg), void *poll_arg) {
    lwip_poll_list.poll = NULL;
}

/******************************************************************************/
// The lwip global functions.

static mp_obj_t mod_lwip_reset() {
    lwip_init();
    lwip_poll_list.poll = NULL;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_lwip_reset_obj, mod_lwip_reset);

static mp_obj_t mod_lwip_callback() {
    if (lwip_poll_list.poll != NULL) {
        lwip_poll_list.poll(lwip_poll_list.poll_arg);
    }
    sys_check_timeouts();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_lwip_callback_obj, mod_lwip_callback);

typedef struct _getaddrinfo_state_t {
    volatile int status;
    volatile ip_addr_t ipaddr;
} getaddrinfo_state_t;

// Callback for incoming DNS requests.
#if LWIP_VERSION_MAJOR < 2
static void lwip_getaddrinfo_cb(const char *name, ip_addr_t *ipaddr, void *arg)
#else
static void lwip_getaddrinfo_cb(const char *name, const ip_addr_t *ipaddr, void *arg)
#endif
{
    getaddrinfo_state_t *state = arg;
    if (ipaddr != NULL) {
        state->status = 1;
        state->ipaddr = *ipaddr;
    } else {
        // error
        state->status = -2;
    }
}

// lwip.getaddrinfo
static mp_obj_t lwip_getaddrinfo(size_t n_args, const mp_obj_t *args) {
    mp_obj_t host_in = args[0], port_in = args[1];
    const char *host = mp_obj_str_get_str(host_in);
    mp_int_t port = mp_obj_get_int(port_in);

    // If constraints were passed then check they are compatible with the supported params
    if (n_args > 2) {
        mp_int_t family = mp_obj_get_int(args[2]);
        mp_int_t type = 0;
        mp_int_t proto = 0;
        mp_int_t flags = 0;
        if (n_args > 3) {
            type = mp_obj_get_int(args[3]);
            if (n_args > 4) {
                proto = mp_obj_get_int(args[4]);
                if (n_args > 5) {
                    flags = mp_obj_get_int(args[5]);
                }
            }
        }
        if (!((family == 0 || family == MOD_NETWORK_AF_INET)
              && (type == 0 || type == MOD_NETWORK_SOCK_STREAM)
              && proto == 0
              && flags == 0)) {
            mp_warning(MP_WARN_CAT(RuntimeWarning), "unsupported getaddrinfo constraints");
        }
    }

    getaddrinfo_state_t state;
    state.status = 0;

    MICROPY_PY_LWIP_ENTER
    #if LWIP_VERSION_MAJOR < 2
    err_t ret = dns_gethostbyname(host, (ip_addr_t *)&state.ipaddr, lwip_getaddrinfo_cb, &state);
    #else
    err_t ret = dns_gethostbyname_addrtype(host, (ip_addr_t *)&state.ipaddr, lwip_getaddrinfo_cb, &state, mp_mod_network_prefer_dns_use_ip_version == 4 ? LWIP_DNS_ADDRTYPE_IPV4_IPV6 : LWIP_DNS_ADDRTYPE_IPV6_IPV4);
    #endif
    MICROPY_PY_LWIP_EXIT

    switch (ret) {
        case ERR_OK:
            // cached
            state.status = 1;
            break;
        case ERR_INPROGRESS:
            while (state.status == 0) {
                poll_sockets();
            }
            break;
        default:
            state.status = ret;
    }

    if (state.status < 0) {
        // TODO: CPython raises gaierror, we raise with native lwIP negative error
        // values, to differentiate from normal errno's at least in such way.
        mp_raise_OSError(state.status);
    }

    ip_addr_t ipcopy = state.ipaddr;
    mp_obj_tuple_t *tuple = MP_OBJ_TO_PTR(mp_obj_new_tuple(5, NULL));
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    tuple->items[4] = lwip_format_inet_addr(&ipcopy, port);
    return mp_obj_new_list(1, (mp_obj_t *)&tuple);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwip_getaddrinfo_obj, 2, 6, lwip_getaddrinfo);

// Debug functions

static mp_obj_t lwip_print_pcbs() {
    tcp_debug_print_pcbs();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(lwip_print_pcbs_obj, lwip_print_pcbs);

static const mp_rom_map_elem_t mp_module_lwip_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_lwip) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&mod_lwip_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_callback), MP_ROM_PTR(&mod_lwip_callback_obj) },
    { MP_ROM_QSTR(MP_QSTR_getaddrinfo), MP_ROM_PTR(&lwip_getaddrinfo_obj) },
    { MP_ROM_QSTR(MP_QSTR_print_pcbs), MP_ROM_PTR(&lwip_print_pcbs_obj) },
    // objects
    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&lwip_socket_type) },
    #ifdef MICROPY_PY_LWIP_SLIP
    { MP_ROM_QSTR(MP_QSTR_slip), MP_ROM_PTR(&lwip_slip_type) },
    #endif
    // class constants
    { MP_ROM_QSTR(MP_QSTR_AF_INET), MP_ROM_INT(MOD_NETWORK_AF_INET) },
    { MP_ROM_QSTR(MP_QSTR_AF_INET6), MP_ROM_INT(MOD_NETWORK_AF_INET6) },

    { MP_ROM_QSTR(MP_QSTR_SOCK_STREAM), MP_ROM_INT(MOD_NETWORK_SOCK_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_SOCK_DGRAM), MP_ROM_INT(MOD_NETWORK_SOCK_DGRAM) },
    #if MICROPY_PY_LWIP_SOCK_RAW
    { MP_ROM_QSTR(MP_QSTR_SOCK_RAW), MP_ROM_INT(MOD_NETWORK_SOCK_RAW) },
    #endif

    { MP_ROM_QSTR(MP_QSTR_SOL_SOCKET), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_SO_REUSEADDR), MP_ROM_INT(SOF_REUSEADDR) },
    { MP_ROM_QSTR(MP_QSTR_SO_BROADCAST), MP_ROM_INT(SOF_BROADCAST) },

    { MP_ROM_QSTR(MP_QSTR_IPPROTO_IP), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_IP_ADD_MEMBERSHIP), MP_ROM_INT(IP_ADD_MEMBERSHIP) },
    { MP_ROM_QSTR(MP_QSTR_IP_DROP_MEMBERSHIP), MP_ROM_INT(IP_DROP_MEMBERSHIP) },

    { MP_ROM_QSTR(MP_QSTR_IPPROTO_TCP), MP_ROM_INT(IP_PROTO_TCP) },
    { MP_ROM_QSTR(MP_QSTR_TCP_NODELAY), MP_ROM_INT(TCP_NODELAY) },
};

static MP_DEFINE_CONST_DICT(mp_module_lwip_globals, mp_module_lwip_globals_table);

const mp_obj_module_t mp_module_lwip = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_lwip_globals,
};

MP_REGISTER_MODULE(MP_QSTR_lwip, mp_module_lwip);

// On LWIP-ports, this is the socket module (replaces extmod/modsocket.c).
MP_REGISTER_EXTENSIBLE_MODULE(MP_QSTR_socket, mp_module_lwip);

MP_REGISTER_ROOT_POINTER(mp_obj_t lwip_slip_stream);

#endif // MICROPY_PY_LWIP
