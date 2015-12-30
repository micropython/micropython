/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Galen Hazelwood
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
#include <errno.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mphal.h"

#include "netutils.h"

#include "lwip/init.h"
#include "lwip/timers.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
//#include "lwip/raw.h"
#include "lwip/dns.h"

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
STATIC lwip_slip_obj_t lwip_slip_obj;

// Declare these early.
void mod_lwip_register_poll(void (*poll)(void *arg), void *poll_arg);
void mod_lwip_deregister_poll(void (*poll)(void *arg), void *poll_arg);

STATIC void slip_lwip_poll(void *netif) {
    slipif_poll((struct netif*)netif);
}

STATIC const mp_obj_type_t lwip_slip_type;

// lwIP SLIP callback functions
sio_fd_t sio_open(u8_t dvnum) {
    // We support singleton SLIP interface, so just return any truish value.
    return (sio_fd_t)1;
}

void sio_send(u8_t c, sio_fd_t fd) {
    mp_obj_type_t *type = mp_obj_get_type(MP_STATE_VM(lwip_slip_stream));
    int error;
    type->stream_p->write(MP_STATE_VM(lwip_slip_stream), &c, 1, &error);
}

u32_t sio_tryread(sio_fd_t fd, u8_t *data, u32_t len) {
    mp_obj_type_t *type = mp_obj_get_type(MP_STATE_VM(lwip_slip_stream));
    int error;
    mp_uint_t out_sz = type->stream_p->read(MP_STATE_VM(lwip_slip_stream), data, len, &error);
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
STATIC mp_obj_t lwip_slip_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    lwip_slip_obj.base.type = &lwip_slip_type;

    MP_STATE_VM(lwip_slip_stream) = args[0];

    ip_addr_t iplocal, ipremote;
    if (!ipaddr_aton(mp_obj_str_get_str(args[1]), &iplocal)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "not a valid local IP"));
    }
    if (!ipaddr_aton(mp_obj_str_get_str(args[2]), &ipremote)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "not a valid remote IP"));
    }

    struct netif *n = &lwip_slip_obj.lwip_netif;
    if (netif_add(n, &iplocal, IP_ADDR_BROADCAST, &ipremote, NULL, slipif_init, ip_input) == NULL) {
       nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "out of memory"));
    }
    netif_set_up(n);
    netif_set_default(n);
    mod_lwip_register_poll(slip_lwip_poll, n);

    return (mp_obj_t)&lwip_slip_obj;
}

STATIC mp_obj_t lwip_slip_status(mp_obj_t self_in) {
    // Null function for now.
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwip_slip_status_obj, lwip_slip_status);

STATIC const mp_map_elem_t lwip_slip_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&lwip_slip_status_obj },
};

STATIC MP_DEFINE_CONST_DICT(lwip_slip_locals_dict, lwip_slip_locals_dict_table);

STATIC const mp_obj_type_t lwip_slip_type = {
    { &mp_type_type },
    .name = MP_QSTR_slip,
    .make_new = lwip_slip_make_new,
    .locals_dict = (mp_obj_t)&lwip_slip_locals_dict,
};

#endif // MICROPY_PY_LWIP_SLIP

/******************************************************************************/
// Table to convert lwIP err_t codes to socket errno codes, from the lwIP
// socket API.

// Extension to lwIP error codes
#define _ERR_BADF -16
static const int error_lookup_table[] = {
    0,             /* ERR_OK          0      No error, everything OK. */
    ENOMEM,        /* ERR_MEM        -1      Out of memory error.     */
    ENOBUFS,       /* ERR_BUF        -2      Buffer error.            */
    EWOULDBLOCK,   /* ERR_TIMEOUT    -3      Timeout                  */
    EHOSTUNREACH,  /* ERR_RTE        -4      Routing problem.         */
    EINPROGRESS,   /* ERR_INPROGRESS -5      Operation in progress    */
    EINVAL,        /* ERR_VAL        -6      Illegal value.           */
    EWOULDBLOCK,   /* ERR_WOULDBLOCK -7      Operation would block.   */
    EADDRINUSE,    /* ERR_USE        -8      Address in use.          */
    EALREADY,      /* ERR_ISCONN     -9      Already connected.       */
    ECONNABORTED,  /* ERR_ABRT       -10     Connection aborted.      */
    ECONNRESET,    /* ERR_RST        -11     Connection reset.        */
    ENOTCONN,      /* ERR_CLSD       -12     Connection closed.       */
    ENOTCONN,      /* ERR_CONN       -13     Not connected.           */
    EIO,           /* ERR_ARG        -14     Illegal argument.        */
    -1,            /* ERR_IF         -15     Low-level netif error    */
    EBADF,         /* _ERR_BADF      -16     Closed socket (null pcb) */
};

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
    } pcb;
    volatile union {
        struct pbuf *pbuf;
        struct tcp_pcb *connection;
    } incoming;
    byte peer[4];
    mp_uint_t peer_port;
    mp_uint_t timeout;
    uint16_t leftover_count;

    uint8_t domain;
    uint8_t type;

    #define STATE_NEW 0
    #define STATE_CONNECTING 1
    #define STATE_CONNECTED 2
    #define STATE_PEER_CLOSED 3
    // Negative value is lwIP error
    int8_t state;
} lwip_socket_obj_t;

static inline void poll_sockets(void) {
    // TODO: Allow to override by ports
    mp_hal_delay_ms(1);
}

/*******************************************************************************/
// Callback functions for the lwIP raw API.

// Callback for incoming UDP packets. We simply stash the packet and the source address,
// in case we need it for recvfrom.
STATIC void _lwip_udp_incoming(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t*)arg;

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
STATIC void _lwip_tcp_error(void *arg, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t*)arg;

    // Pass the error code back via the connection variable.
    socket->state = err;
    // If we got here, the lwIP stack either has deallocated or will deallocate the pcb.
    socket->pcb.tcp = NULL;
}

// Callback for tcp connection requests. Error code err is unused. (See tcp.h)
STATIC err_t _lwip_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t*)arg;

    socket->state = STATE_CONNECTED;
    return ERR_OK;
}

// Callback for incoming tcp connections.
STATIC err_t _lwip_tcp_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t*)arg;

    if (socket->incoming.connection != NULL) {
        // We need to handle this better. This single-level structure makes the
        // backlog setting kind of pointless. FIXME
        return ERR_BUF;
    } else {
        socket->incoming.connection = newpcb;
        return ERR_OK;
    }
}

// Callback for inbound tcp packets.
STATIC err_t _lwip_tcp_recv(void *arg, struct tcp_pcb *tcpb, struct pbuf *p, err_t err) {
    lwip_socket_obj_t *socket = (lwip_socket_obj_t*)arg;

    if (p == NULL) {
        // Other side has closed connection.
        socket->state = STATE_PEER_CLOSED;
        return ERR_OK;
    } else if (socket->incoming.pbuf != NULL) {
        // No room in the inn, let LWIP know it's still responsible for delivery later
        return ERR_BUF;
    }
    socket->incoming.pbuf = p;
    return ERR_OK;
}

STATIC uint8_t lwip_dns_returned;
STATIC uint8_t lwip_dns_result[4];

// Callback for incoming DNS requests. Just set our results.
STATIC void _lwip_dns_incoming(const char *name, ip_addr_t *addr, void *callback_arg) {
    if (addr != NULL) {
        lwip_dns_returned = 1;
        memcpy(lwip_dns_result, addr, sizeof(lwip_dns_result));
    } else {
        lwip_dns_returned = 2;
    }
}

/*******************************************************************************/
// Functions for socket send/recieve operations. Socket send/recv and friends call
// these to do the work.

// Helper function for send/sendto to handle UDP packets.
STATIC mp_uint_t lwip_udp_send(lwip_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    if (len > 0xffff) {
        // Any packet that big is probably going to fail the pbuf_alloc anyway, but may as well try
        len = 0xffff;
    }

    // FIXME: maybe PBUF_ROM?
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        *_errno = ENOMEM;
        return -1;
    }

    memcpy(p->payload, buf, len);

    err_t err;
    if (ip == NULL) {
        err = udp_send(socket->pcb.udp, p);
    } else {
        ip_addr_t dest;
        IP4_ADDR(&dest, ip[0], ip[1], ip[2], ip[3]);
        err = udp_sendto(socket->pcb.udp, p, &dest, port);
    }

    pbuf_free(p);

    if (err != ERR_OK) {
        *_errno = error_lookup_table[-err];
        return -1;
    }

    return len;
}

// Helper function for recv/recvfrom to handle UDP packets
STATIC mp_uint_t lwip_udp_receive(lwip_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {

    if (socket->incoming.pbuf == NULL) {
        if (socket->timeout != -1) {
            for (mp_uint_t retries = socket->timeout / 100; retries--;) {
                mp_hal_delay_ms(100);
                if (socket->incoming.pbuf != NULL) break;
            }
            if (socket->incoming.pbuf == NULL) {
                *_errno = ETIMEDOUT;
                return -1;
            }
        } else {
            while (socket->incoming.pbuf == NULL) {
                mp_hal_delay_ms(100);
            }
        }
    }

    if (ip != NULL) {
        memcpy(ip, &socket->peer, sizeof(socket->peer));
        *port = socket->peer_port;
    }

    struct pbuf *p = socket->incoming.pbuf;

    u16_t result = pbuf_copy_partial(p, buf, ((p->tot_len > len) ? len : p->tot_len), 0);
    pbuf_free(p);
    socket->incoming.pbuf = NULL;

    return (mp_uint_t) result;
}

// Helper function for send/sendto to handle TCP packets
STATIC mp_uint_t lwip_tcp_send(lwip_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    u16_t available = tcp_sndbuf(socket->pcb.tcp);
    u16_t write_len = MIN(available, len);

    err_t err = tcp_write(socket->pcb.tcp, buf, write_len, TCP_WRITE_FLAG_COPY);

    if (err != ERR_OK) {
        *_errno = error_lookup_table[-err];
        return -1;
    }

    return write_len;
}

// Helper function for recv/recvfrom to handle TCP packets
STATIC mp_uint_t lwip_tcp_receive(lwip_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {

    if (socket->state == STATE_PEER_CLOSED) {
        return 0;
    }

    if (socket->incoming.pbuf == NULL) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (socket->incoming.pbuf == NULL) {
            if (socket->timeout != -1 && mp_hal_ticks_ms() - start > socket->timeout) {
                *_errno = ETIMEDOUT;
                return -1;
            }
            poll_sockets();
        }
    }

    struct pbuf *p = socket->incoming.pbuf;

    if (socket->leftover_count == 0) {
        socket->leftover_count = p->tot_len;
    }

    u16_t result = pbuf_copy_partial(p, buf, ((socket->leftover_count >= len) ? len : socket->leftover_count), (p->tot_len - socket->leftover_count));
    if (socket->leftover_count > len) {
        // More left over...
        socket->leftover_count -= len;
    } else {
        pbuf_free(p);
        socket->incoming.pbuf = NULL;
        socket->leftover_count = 0;
    }

    tcp_recved(socket->pcb.tcp, result);
    return (mp_uint_t) result;
}

/*******************************************************************************/
// The socket functions provided by lwip.socket.

STATIC const mp_obj_type_t lwip_socket_type;

// FIXME: Only supports two arguments at present
STATIC mp_obj_t lwip_socket_make_new(mp_obj_t type_in, mp_uint_t n_args,
    mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    lwip_socket_obj_t *socket = m_new_obj_with_finaliser(lwip_socket_obj_t);
    socket->base.type = (mp_obj_t)&lwip_socket_type;
    socket->domain = MOD_NETWORK_AF_INET;
    socket->type = MOD_NETWORK_SOCK_STREAM;
    if (n_args >= 1) {
        socket->domain = mp_obj_get_int(args[0]);
        if (n_args >= 2) {
            socket->type = mp_obj_get_int(args[1]);
        }
    }

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: socket->pcb.tcp = tcp_new(); break;
        case MOD_NETWORK_SOCK_DGRAM: socket->pcb.udp = udp_new(); break;
        //case MOD_NETWORK_SOCK_RAW: socket->pcb.raw = raw_new(); break;
        default: nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EINVAL)));
    }

    if (socket->pcb.tcp == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ENOMEM)));
    }

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            // Register the socket object as our callback argument.
            tcp_arg(socket->pcb.tcp, (void*)socket);
            // Register our error callback.
            tcp_err(socket->pcb.tcp, _lwip_tcp_error);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            // Register our receive callback now. Since UDP sockets don't require binding or connection
            // before use, there's no other good time to do it.
            udp_recv(socket->pcb.udp, _lwip_udp_incoming, (void*)socket);
            break;
        }
    }

    socket->incoming.pbuf = NULL;
    socket->timeout = -1;
    socket->state = STATE_NEW;
    socket->leftover_count = 0;
    return socket;
}

STATIC mp_obj_t lwip_socket_close(mp_obj_t self_in) {
    lwip_socket_obj_t *socket = self_in;
    bool socket_is_listener = false;

    if (socket->pcb.tcp == NULL) {
        return mp_const_none;
    }
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            if (socket->pcb.tcp->state == LISTEN) {
                socket_is_listener = true;
            }
            if (tcp_close(socket->pcb.tcp) != ERR_OK) {
                tcp_abort(socket->pcb.tcp);
            }
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: udp_remove(socket->pcb.udp); break;
        //case MOD_NETWORK_SOCK_RAW: raw_remove(socket->pcb.raw); break;
    }
    socket->pcb.tcp = NULL;
    socket->state = _ERR_BADF;
    if (socket->incoming.pbuf != NULL) {
        if (!socket_is_listener) {
            pbuf_free(socket->incoming.pbuf);
        } else {
            tcp_abort(socket->incoming.connection);
        }
        socket->incoming.pbuf = NULL;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwip_socket_close_obj, lwip_socket_close);

STATIC mp_obj_t lwip_socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = self_in;

    uint8_t ip[NETUTILS_IPV4ADDR_BUFSIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    ip_addr_t bind_addr;
    IP4_ADDR(&bind_addr, ip[0], ip[1], ip[2], ip[3]);

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
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error_lookup_table[-err])));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_bind_obj, lwip_socket_bind);

STATIC mp_obj_t lwip_socket_listen(mp_obj_t self_in, mp_obj_t backlog_in) {
    lwip_socket_obj_t *socket = self_in;
    mp_int_t backlog = mp_obj_get_int(backlog_in);

    if (socket->pcb.tcp == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EBADF)));
    }
    if (socket->type != MOD_NETWORK_SOCK_STREAM) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EOPNOTSUPP)));
    }

    struct tcp_pcb *new_pcb = tcp_listen_with_backlog(socket->pcb.tcp, (u8_t)backlog);
    if (new_pcb == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ENOMEM)));
    }
    socket->pcb.tcp = new_pcb;
    tcp_accept(new_pcb, _lwip_tcp_accept);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_listen_obj, lwip_socket_listen);

STATIC mp_obj_t lwip_socket_accept(mp_obj_t self_in) {
    lwip_socket_obj_t *socket = self_in;

    if (socket->pcb.tcp == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EBADF)));
    }
    if (socket->type != MOD_NETWORK_SOCK_STREAM) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EOPNOTSUPP)));
    }
    // I need to do this because "tcp_accepted", later, is a macro.
    struct tcp_pcb *listener = socket->pcb.tcp;
    if (listener->state != LISTEN) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EINVAL)));
    }

    // accept incoming connection
    if (socket->incoming.connection == NULL) {
        if (socket->timeout != -1) {
            for (mp_uint_t retries = socket->timeout / 100; retries--;) {
                mp_hal_delay_ms(100);
                if (socket->incoming.connection != NULL) break;
            }
            if (socket->incoming.connection == NULL) {
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ETIMEDOUT)));
            }
        } else {
            while (socket->incoming.connection == NULL) {
                mp_hal_delay_ms(100);
            }
        }
    }

    // create new socket object
    lwip_socket_obj_t *socket2 = m_new_obj_with_finaliser(lwip_socket_obj_t);
    socket2->base.type = (mp_obj_t)&lwip_socket_type;

    // We get a new pcb handle...
    socket2->pcb.tcp = socket->incoming.connection;
    socket->incoming.connection = NULL;

    // ...and set up the new socket for it.
    socket2->domain = MOD_NETWORK_AF_INET;
    socket2->type = MOD_NETWORK_SOCK_STREAM;
    socket2->incoming.pbuf = NULL;
    socket2->timeout = socket->timeout;
    socket2->state = STATE_CONNECTED;
    socket2->leftover_count = 0;
    tcp_arg(socket2->pcb.tcp, (void*)socket2);
    tcp_err(socket2->pcb.tcp, _lwip_tcp_error);
    tcp_recv(socket2->pcb.tcp, _lwip_tcp_recv);

    tcp_accepted(listener);

    // make the return value
    uint8_t ip[NETUTILS_IPV4ADDR_BUFSIZE];
    memcpy(ip, &(socket2->pcb.tcp->remote_ip), sizeof(ip));
    mp_uint_t port = (mp_uint_t)socket2->pcb.tcp->remote_port;
    mp_obj_tuple_t *client = mp_obj_new_tuple(2, NULL);
    client->items[0] = socket2;
    client->items[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);

    return client;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwip_socket_accept_obj, lwip_socket_accept);

STATIC mp_obj_t lwip_socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = self_in;

    if (socket->pcb.tcp == NULL) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EBADF)));
    }

    // get address
    uint8_t ip[NETUTILS_IPV4ADDR_BUFSIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    ip_addr_t dest;
    IP4_ADDR(&dest, ip[0], ip[1], ip[2], ip[3]);

    err_t err = ERR_ARG;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            if (socket->state != STATE_NEW) {
                if (socket->state == STATE_CONNECTED) {
                    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EALREADY)));
                } else {
                    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EINPROGRESS)));
                }
            }
            // Register our recieve callback.
            tcp_recv(socket->pcb.tcp, _lwip_tcp_recv);
            socket->state = STATE_CONNECTING;
            err = tcp_connect(socket->pcb.tcp, &dest, port, _lwip_tcp_connected);
            if (err != ERR_OK) {
                socket->state = STATE_NEW;
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error_lookup_table[-err])));
            }
            socket->peer_port = (mp_uint_t)port;
            memcpy(socket->peer, &dest, sizeof(socket->peer));
            // And now we wait...
            if (socket->timeout != -1) {
                for (mp_uint_t retries = socket->timeout / 100; retries--;) {
                    mp_hal_delay_ms(100);
                    if (socket->state != STATE_CONNECTING) break;
                }
                if (socket->state == STATE_CONNECTING) {
                    nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ETIMEDOUT)));
                }
            } else {
                while (socket->state == STATE_CONNECTING) {
                    mp_hal_delay_ms(100);
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
    }

    if (err != ERR_OK) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(error_lookup_table[-err])));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_connect_obj, lwip_socket_connect);

STATIC mp_obj_t lwip_socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    lwip_socket_obj_t *socket = self_in;
    int _errno;

    if (socket->pcb.tcp == NULL) {
        // not connected
        _errno = error_lookup_table[-(socket->state)];
        socket->state = _ERR_BADF;
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, bufinfo.buf, bufinfo.len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            ret = lwip_udp_send(socket, bufinfo.buf, bufinfo.len, NULL, 0, &_errno);
            break;
        }
    }
    if (ret == -1) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_send_obj, lwip_socket_send);

STATIC mp_obj_t lwip_socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    lwip_socket_obj_t *socket = self_in;
    int _errno;

    if (socket->pcb.tcp == NULL) {
        // not connected
        _errno = error_lookup_table[-(socket->state)];
        socket->state = _ERR_BADF;
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_receive(socket, (byte*)vstr.buf, len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            ret = lwip_udp_receive(socket, (byte*)vstr.buf, len, NULL, NULL, &_errno);
            break;
        }
    }
    if (ret == -1) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    if (ret == 0) {
        return mp_const_empty_bytes;
    }
    vstr.len = ret;
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_recv_obj, lwip_socket_recv);

STATIC mp_obj_t lwip_socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    lwip_socket_obj_t *socket = self_in;
    int _errno;

    if (socket->pcb.tcp == NULL) {
        // not connected
        _errno = error_lookup_table[-(socket->state)];
        socket->state = _ERR_BADF;
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);

    uint8_t ip[NETUTILS_IPV4ADDR_BUFSIZE];
    mp_uint_t port = netutils_parse_inet_addr(addr_in, ip, NETUTILS_BIG);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, bufinfo.buf, bufinfo.len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            ret = lwip_udp_send(socket, bufinfo.buf, bufinfo.len, ip, port, &_errno);
            break;
        }
    }
    if (ret == -1) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    return mp_obj_new_int_from_uint(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(lwip_socket_sendto_obj, lwip_socket_sendto);

STATIC mp_obj_t lwip_socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    lwip_socket_obj_t *socket = self_in;
    int _errno;

    if (socket->pcb.tcp == NULL) {
        // not connected
        _errno = error_lookup_table[-(socket->state)];
        socket->state = _ERR_BADF;
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    mp_int_t len = mp_obj_get_int(len_in);
    vstr_t vstr;
    vstr_init_len(&vstr, len);
    byte ip[4];
    mp_uint_t port;

    mp_uint_t ret = 0;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            memcpy(ip, &socket->peer, sizeof(socket->peer));
            port = (mp_uint_t) socket->peer_port;
            ret = lwip_tcp_receive(socket, (byte*)vstr.buf, len, &_errno);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            ret = lwip_udp_receive(socket, (byte*)vstr.buf, len, ip, &port, &_errno);
            break;
        }
    }
    if (ret == -1) {
        nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(_errno)));
    }

    mp_obj_t tuple[2];
    if (ret == 0) {
        tuple[0] = mp_const_empty_bytes;
    } else {
        vstr.len = ret;
        tuple[0] = mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
    }
    tuple[1] = netutils_format_inet_addr(ip, port, NETUTILS_BIG);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_recvfrom_obj, lwip_socket_recvfrom);

STATIC mp_obj_t lwip_socket_settimeout(mp_obj_t self_in, mp_obj_t timeout_in) {
    lwip_socket_obj_t *socket = self_in;
    mp_uint_t timeout;
    if (timeout_in == mp_const_none) {
        timeout = -1;
    } else {
        #if MICROPY_PY_BUILTIN_FLOAT
        timeout = 1000 * mp_obj_get_float(timeout_in);
        #else
        timeout = 1000 * mp_obj_get_int(timeout_in);
        #endif
    }
    socket->timeout = timeout;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_socket_settimeout_obj, lwip_socket_settimeout);

STATIC const mp_map_elem_t lwip_socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&lwip_socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&lwip_socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&lwip_socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&lwip_socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&lwip_socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&lwip_socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&lwip_socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&lwip_socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto), (mp_obj_t)&lwip_socket_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom), (mp_obj_t)&lwip_socket_recvfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_settimeout), (mp_obj_t)&lwip_socket_settimeout_obj },
};
STATIC MP_DEFINE_CONST_DICT(lwip_socket_locals_dict, lwip_socket_locals_dict_table);

STATIC const mp_obj_type_t lwip_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .make_new = lwip_socket_make_new,
    .locals_dict = (mp_obj_t)&lwip_socket_locals_dict,
};

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
    void (* poll)(void *arg);
    void *poll_arg;
} nic_poll_t;

STATIC nic_poll_t lwip_poll_list;

void mod_lwip_register_poll(void (* poll)(void *arg), void *poll_arg) {
    lwip_poll_list.poll = poll;
    lwip_poll_list.poll_arg = poll_arg;
}

void mod_lwip_deregister_poll(void (* poll)(void *arg), void *poll_arg) {
    lwip_poll_list.poll = NULL;
}

/******************************************************************************/
// The lwip global functions.

STATIC mp_obj_t mod_lwip_reset() {
    lwip_init();
    lwip_poll_list.poll = NULL;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_lwip_reset_obj, mod_lwip_reset);

STATIC mp_obj_t mod_lwip_callback() {
    if (lwip_poll_list.poll != NULL) {
        lwip_poll_list.poll(lwip_poll_list.poll_arg);
    }
    sys_check_timeouts();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(mod_lwip_callback_obj, mod_lwip_callback);

// lwip.getaddrinfo
STATIC mp_obj_t lwip_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    mp_int_t port = mp_obj_get_int(port_in);

    ip_addr_t result;
    lwip_dns_returned = 0;

    switch (dns_gethostbyname(host, &result, _lwip_dns_incoming, NULL)) {
        case ERR_OK: {
             break;
        }
        case ERR_INPROGRESS: {
            while(!lwip_dns_returned) {
                mp_hal_delay_ms(100);
            }
            if (lwip_dns_returned == 2) {
                nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ENOENT)));
            }
            break;
        }
        default: {
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(ENOENT)));
        }
    }

    uint8_t out_ip[NETUTILS_IPV4ADDR_BUFSIZE];
    memcpy(out_ip, lwip_dns_result, sizeof(lwip_dns_result));
    mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);
    tuple->items[0] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET);
    tuple->items[1] = MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM);
    tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
    tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
    tuple->items[4] = netutils_format_inet_addr(out_ip, port, NETUTILS_BIG);
    return mp_obj_new_list(1, (mp_obj_t*)&tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(lwip_getaddrinfo_obj, lwip_getaddrinfo);

#ifdef MICROPY_PY_LWIP

STATIC const mp_map_elem_t mp_module_lwip_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_lwip) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_reset), (mp_obj_t)&mod_lwip_reset_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_callback), (mp_obj_t)&mod_lwip_callback_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&lwip_getaddrinfo_obj },
    // objects
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&lwip_socket_type },
#ifdef MICROPY_PY_LWIP_SLIP
    { MP_OBJ_NEW_QSTR(MP_QSTR_slip), (mp_obj_t)&lwip_slip_type },
#endif
    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_AF_INET6) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_DGRAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_RAW), MP_OBJ_NEW_SMALL_INT(MOD_NETWORK_SOCK_RAW) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_lwip_globals, mp_module_lwip_globals_table);

const mp_obj_module_t mp_module_lwip = {
    .base = { &mp_type_module },
    .name = MP_QSTR_lwip,
    .globals = (mp_obj_dict_t*)&mp_module_lwip_globals,
};

#endif // MICROPY_PY_LWIP
