/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2015 Galen Hazelwood
 * Copyright (c) 2015-2017 Paul Sokolovsky
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
 * Copyright (c) 2022 Jeff Epler for Adafruit Industries
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

#include "shared-bindings/socketpool/Socket.h"

#include "py/gc.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "shared-bindings/socketpool/SocketPool.h"
#include "shared/runtime/interrupt_char.h"
#include "shared/netutils/netutils.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"
#include "supervisor/workflow.h"

#include "lwip/dns.h"
#include "lwip/err.h"
#include "lwip/igmp.h"
#include "lwip/init.h"
#include "lwip/netdb.h"
#include "lwip/priv/tcp_priv.h"
#include "lwip/raw.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"

#define MICROPY_PY_LWIP_SOCK_RAW (1)

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

/******************************************************************************/
// Table to convert lwIP err_t codes to socket errno codes, from the lwIP
// socket API.

// Extension to lwIP error codes
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

/*******************************************************************************/
// The socket object provided by lwip.socket.

#define MOD_NETWORK_AF_INET (SOCKETPOOL_AF_INET)
#define MOD_NETWORK_AF_INET6 (SOCKETPOOL_AF_INET6)

#define MOD_NETWORK_SOCK_STREAM (SOCKETPOOL_SOCK_STREAM)
#define MOD_NETWORK_SOCK_DGRAM (SOCKETPOOL_SOCK_DGRAM)
#define MOD_NETWORK_SOCK_RAW (SOCKETPOOL_SOCK_RAW)

#define MAX_SOCKETS (8)

static inline void poll_sockets(void) {
    #ifdef MICROPY_EVENT_POLL_HOOK
    MICROPY_EVENT_POLL_HOOK;
    #else
    RUN_BACKGROUND_TASKS;
    if (MP_STATE_THREAD(mp_pending_exception) != MP_OBJ_NULL) {
        mp_handle_pending(true);
    }
    mp_hal_delay_ms(1);
    #endif
}

STATIC struct tcp_pcb *volatile *lwip_socket_incoming_array(socketpool_socket_obj_t *socket) {
    if (socket->incoming.connection.alloc == 0) {
        return &socket->incoming.connection.tcp.item;
    } else {
        return &socket->incoming.connection.tcp.array[0];
    }
}

STATIC void lwip_socket_free_incoming(socketpool_socket_obj_t *socket) {
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

/*******************************************************************************/
// Callback functions for the lwIP raw API.

static inline void exec_user_callback(socketpool_socket_obj_t *socket) {
    #if 0
    if (socket->callback != MP_OBJ_NULL) {
        // Schedule the user callback to execute outside the lwIP context
        mp_sched_schedule(socket->callback, MP_OBJ_FROM_PTR(socket));
    }
    #endif
    supervisor_workflow_request_background();
}

#if MICROPY_PY_LWIP_SOCK_RAW
// Callback for incoming raw packets.
#if LWIP_VERSION_MAJOR < 2
STATIC u8_t _lwip_raw_incoming(void *arg, struct raw_pcb *pcb, struct pbuf *p, ip_addr_t *addr)
#else
STATIC u8_t _lwip_raw_incoming(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr)
#endif
{
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;

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
STATIC void _lwip_udp_incoming(void *arg, struct udp_pcb *upcb, struct pbuf *p, ip_addr_t *addr, u16_t port)
#else
STATIC void _lwip_udp_incoming(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *addr, u16_t port)
#endif
{
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;

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
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;

    // Free any incoming buffers or connections that are stored
    lwip_socket_free_incoming(socket);
    // Pass the error code back via the connection variable.
    socket->state = err;
    // If we got here, the lwIP stack either has deallocated or will deallocate the pcb.
    socket->pcb.tcp = NULL;
}

// Callback for tcp connection requests. Error code err is unused. (See tcp.h)
STATIC err_t _lwip_tcp_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;

    socket->state = STATE_CONNECTED;
    return ERR_OK;
}

// Handle errors (eg connection aborted) on TCP PCBs that have been put on the
// accept queue but are not yet actually accepted.
STATIC void _lwip_tcp_err_unaccepted(void *arg, err_t err) {
    struct tcp_pcb *pcb = (struct tcp_pcb *)arg;

    // The ->connected entry is repurposed to store the parent socket; this is safe
    // because it's only ever used by lwIP if tcp_connect is called on the TCP PCB.
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)pcb->connected;

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
STATIC err_t _lwip_tcp_recv_unaccepted(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err) {
    return ERR_BUF;
}

// Callback for incoming tcp connections.
STATIC err_t _lwip_tcp_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // err can be ERR_MEM to notify us that there was no memory for an incoming connection
    if (err != ERR_OK) {
        return ERR_OK;
    }

    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;
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
STATIC err_t _lwip_tcp_recv(void *arg, struct tcp_pcb *tcpb, struct pbuf *p, err_t err) {
    socketpool_socket_obj_t *socket = (socketpool_socket_obj_t *)arg;

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
STATIC mp_uint_t lwip_raw_udp_send(socketpool_socket_obj_t *socket, const byte *buf, mp_uint_t len, ip_addr_t *dest, uint32_t port, int *_errno) {
    if (len > 0xffff) {
        // Any packet that big is probably going to fail the pbuf_alloc anyway, but may as well try
        len = 0xffff;
    }

    MICROPY_PY_LWIP_ENTER

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        MICROPY_PY_LWIP_EXIT
        *_errno = MP_ENOMEM;
        return -1;
    }

    memcpy(p->payload, buf, len);

    err_t err;
    if (dest == NULL) {
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
            err = raw_sendto(socket->pcb.raw, p, dest);
        } else
        #endif
        {
            err = udp_sendto(socket->pcb.udp, p, dest, port);
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
STATIC mp_uint_t lwip_raw_udp_receive(socketpool_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, uint32_t *port, int *_errno) {

    if (socket->incoming.pbuf == NULL) {
        if (socket->timeout == 0) {
            // Non-blocking socket.
            *_errno = MP_EAGAIN;
            return -1;
        }

        // Wait for data to arrive on UDP socket.
        mp_uint_t start = mp_hal_ticks_ms();
        while (socket->incoming.pbuf == NULL) {
            if (socket->timeout != (unsigned)-1 && mp_hal_ticks_ms() - start > socket->timeout) {
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
STATIC mp_uint_t lwip_tcp_send(socketpool_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    // Check for any pending errors
    STREAM_ERROR_CHECK(socket);

    MICROPY_PY_LWIP_ENTER

    u16_t available = tcp_sndbuf(socket->pcb.tcp);

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
        while (socket->state >= STATE_CONNECTED && (available = tcp_sndbuf(socket->pcb.tcp)) < 16) {
            MICROPY_PY_LWIP_EXIT
            if (socket->timeout != (unsigned)-1 && mp_hal_ticks_ms() - start > socket->timeout) {
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
        if (err == ERR_MEM && write_len > TCP_MSS) {
            // Decreasing the amount sent to the next lower number of MSS
            write_len = (write_len - 1) / TCP_MSS * TCP_MSS;
            continue;
        }
        err = tcp_output(socket->pcb.tcp);
        if (err != ERR_OK) {
            break;
        }
        MICROPY_PY_LWIP_EXIT
        mp_hal_delay_ms(50);
        MICROPY_PY_LWIP_REENTER
    }

    // If the output buffer is getting full then send the data to the lower layers
    if (err == ERR_OK && tcp_sndbuf(socket->pcb.tcp) < TCP_SND_BUF / 4) {
        err = tcp_output(socket->pcb.tcp);
    }

    MICROPY_PY_LWIP_EXIT

    if (err != ERR_OK) {
        *_errno = error_lookup_table[-err];
        return MP_STREAM_ERROR;
    }

    return write_len;
}

// Helper function for recv/recvfrom to handle TCP packets
STATIC mp_uint_t lwip_tcp_receive(socketpool_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
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
            if (socket->timeout != (unsigned)-1 && mp_hal_ticks_ms() - start > socket->timeout) {
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


STATIC socketpool_socket_obj_t *open_socket_objs[MAX_SOCKETS];
STATIC bool user_socket[MAX_SOCKETS];

void socket_user_reset(void) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] && user_socket[i]) {
            socketpool_socket_close(open_socket_objs[i]);
            open_socket_objs[i] = NULL;
            user_socket[i] = false;
        }
    }
}

// The writes below send an event to the socket select task so that it redoes the
// select with the new open socket set.

STATIC bool register_open_socket(socketpool_socket_obj_t *obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (!open_socket_objs[i]) {
            open_socket_objs[i] = obj;
            DEBUG_printf("register_open_socket(%p) -> %d\n", obj, i);
            user_socket[i] = false;
            return true;
        }
    }
    DEBUG_printf("register_open_socket(%p) fails due to full table\n", obj);
    return false;
}

STATIC void unregister_open_socket(socketpool_socket_obj_t *obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] == obj) {
            DEBUG_printf("unregister_open_socket(%p) clears %d\n", obj, i);
            open_socket_objs[i] = NULL;
            user_socket[i] = false;
            return;
        }
    }
    DEBUG_printf("unregister_open_socket(%p) fails due to missing entry\n", obj);
}

STATIC void mark_user_socket(socketpool_socket_obj_t *obj) {
    for (size_t i = 0; i < MP_ARRAY_SIZE(open_socket_objs); i++) {
        if (open_socket_objs[i] == obj) {
            DEBUG_printf("mark_user_socket(%p) -> %d\n", obj, i);
            user_socket[i] = true;
            return;
        }
    }
    DEBUG_printf("mark_user_socket(%p) fails due to missing entry\n", obj);
}

bool socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type,
    socketpool_socket_obj_t *socket) {

    if (!register_open_socket(socket)) {
        DEBUG_printf("collecting garbage to open socket\n");
        gc_collect();
        if (!register_open_socket(socket)) {
            return false;
        }
    }

    socket->timeout = -1;
    socket->recv_offset = 0;
    socket->domain = SOCKETPOOL_AF_INET;
    socket->type = type;
    socket->callback = MP_OBJ_NULL;
    socket->state = STATE_NEW;

    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM:
            socket->pcb.tcp = tcp_new();
            socket->incoming.connection.alloc = 0;
            socket->incoming.connection.tcp.item = NULL;
            break;
        case SOCKETPOOL_SOCK_DGRAM:
            socket->pcb.udp = udp_new();
            socket->incoming.pbuf = NULL;
            break;
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW: {
            socket->pcb.raw = raw_new(0);
            break;
        }
        #endif
        default:
            return false;
    }

    if (socket->pcb.tcp == NULL) {
        return false;
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
    return true;
}

socketpool_socket_obj_t *common_hal_socketpool_socket(socketpool_socketpool_obj_t *self,
    socketpool_socketpool_addressfamily_t family, socketpool_socketpool_sock_t type) {
    if (family != SOCKETPOOL_AF_INET) {
        mp_raise_NotImplementedError(translate("Only IPv4 sockets supported"));
    }

    // we must allocate sockets long-lived because we depend on their object-identity
    socketpool_socket_obj_t *socket = m_new_ll_obj_with_finaliser(socketpool_socket_obj_t);
    socket->base.type = &socketpool_socket_type;

    if (!socketpool_socket(self, family, type, socket)) {
        mp_raise_RuntimeError(translate("Out of sockets"));
    }
    mark_user_socket(socket);
    return socket;
}

int socketpool_socket_accept(socketpool_socket_obj_t *self, uint8_t *ip, uint32_t *port, socketpool_socket_obj_t *accepted) {
    if (self->type != MOD_NETWORK_SOCK_STREAM) {
        return -MP_EOPNOTSUPP;
    }

    if (common_hal_socketpool_socket_get_closed(self)) {
        return -MP_EBADF;
    }

    MICROPY_PY_LWIP_ENTER

    if (self->pcb.tcp == NULL) {
        MICROPY_PY_LWIP_EXIT
        return -MP_EBADF;
    }

    // I need to do this because "tcp_accepted", later, is a macro.
    struct tcp_pcb *listener = self->pcb.tcp;
    if (listener->state != LISTEN) {
        MICROPY_PY_LWIP_EXIT
        return -MP_EINVAL;
    }

    // accept incoming connection
    struct tcp_pcb *volatile *incoming_connection = &lwip_socket_incoming_array(self)[self->incoming.connection.iget];
    if (*incoming_connection == NULL) {
        if (self->timeout == 0) {
            MICROPY_PY_LWIP_EXIT
            return -MP_EAGAIN;
        } else if (self->timeout != (unsigned)-1) {
            mp_uint_t retries = self->timeout / 100;
            while (*incoming_connection == NULL && !mp_hal_is_interrupted()) {
                MICROPY_PY_LWIP_EXIT
                if (retries-- == 0) {
                    return -MP_ETIMEDOUT;
                }
                mp_hal_delay_ms(100);
                MICROPY_PY_LWIP_REENTER
            }
        } else {
            while (*incoming_connection == NULL && !mp_hal_is_interrupted()) {
                MICROPY_PY_LWIP_EXIT
                poll_sockets();
                MICROPY_PY_LWIP_REENTER
            }
        }
    }

    if (*incoming_connection == NULL) {
        // We were interrupted.
        return 0;
    }

    // Close the accepted socket because we have another we accepted.
    if (!common_hal_socketpool_socket_get_closed(accepted)) {
        common_hal_socketpool_socket_close(accepted);
    }

    // We get a new pcb handle...
    accepted->pcb.tcp = *incoming_connection;
    if (++self->incoming.connection.iget >= self->incoming.connection.alloc) {
        self->incoming.connection.iget = 0;
    }
    *incoming_connection = NULL;

    // ...and set up the new socket for it.
    accepted->domain = MOD_NETWORK_AF_INET;
    accepted->type = MOD_NETWORK_SOCK_STREAM;
    accepted->incoming.pbuf = NULL;
    accepted->timeout = self->timeout;
    accepted->state = STATE_CONNECTED;
    accepted->recv_offset = 0;
    accepted->callback = MP_OBJ_NULL;
    tcp_arg(accepted->pcb.tcp, (void *)accepted);
    tcp_err(accepted->pcb.tcp, _lwip_tcp_error);
    tcp_recv(accepted->pcb.tcp, _lwip_tcp_recv);

    tcp_accepted(listener);

    MICROPY_PY_LWIP_EXIT

    // output values
    memcpy(ip, &(accepted->pcb.tcp->remote_ip), NETUTILS_IPV4ADDR_BUFSIZE);
    *port = (mp_uint_t)accepted->pcb.tcp->remote_port;

    return 1;
}

socketpool_socket_obj_t *common_hal_socketpool_socket_accept(socketpool_socket_obj_t *socket,
    uint8_t *ip, uint32_t *port) {
    // Create new socket object, do it here because we must not raise an out-of-memory
    // exception when the LWIP concurrency lock is held
    socketpool_socket_obj_t *accepted = m_new_ll_obj_with_finaliser(socketpool_socket_obj_t);
    socketpool_socket_reset(accepted);

    int ret = socketpool_socket_accept(socket, ip, port, accepted);

    if (ret <= 0) {
        m_del_obj(socketpool_socket_obj_t, accepted);
        if (ret == 0) {
            // Interrupted.
            return mp_const_none;
        }
        mp_raise_OSError(-ret);
    }

    DEBUG_printf("registering socket in socketpool_socket_accept()\n");
    if (!register_open_socket(accepted)) {
        DEBUG_printf("collecting garbage to open socket\n");
        gc_collect();
        if (!register_open_socket(accepted)) {
            mp_raise_RuntimeError(translate("Out of sockets"));
        }
    }
    mark_user_socket(accepted);

    return MP_OBJ_FROM_PTR(accepted);
}

bool common_hal_socketpool_socket_bind(socketpool_socket_obj_t *socket,
    const char *host, size_t hostlen, uint32_t port) {

    // get address
    ip_addr_t bind_addr;
    const ip_addr_t *bind_addr_ptr = &bind_addr;
    if (hostlen > 0) {
        socketpool_resolve_host_raise(socket->pool, host, &bind_addr);
    } else {
        bind_addr_ptr = IP_ANY_TYPE;
    }
    ip_set_option(socket->pcb.ip, SOF_REUSEADDR);

    err_t err = ERR_ARG;
    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM: {
            err = tcp_bind(socket->pcb.tcp, bind_addr_ptr, port);
            break;
        }
        case MOD_NETWORK_SOCK_DGRAM: {
            err = udp_bind(socket->pcb.udp, bind_addr_ptr, port);
            break;
        }
    }

    if (err != ERR_OK) {
        mp_raise_OSError(error_lookup_table[-err]);
    }

    return mp_const_none;
}

STATIC err_t _lwip_tcp_close_poll(void *arg, struct tcp_pcb *pcb) {
    // Connection has not been cleanly closed so just abort it to free up memory
    tcp_poll(pcb, NULL, 0);
    tcp_abort(pcb);
    return ERR_OK;
}

void socketpool_socket_close(socketpool_socket_obj_t *socket) {
    unregister_open_socket(socket);
    MICROPY_PY_LWIP_ENTER
    if (socket->pcb.tcp == NULL) { // already closed
        MICROPY_PY_LWIP_EXIT
        return;
    }
    lwip_socket_free_incoming(socket);
    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM: {
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
        case SOCKETPOOL_SOCK_DGRAM:
            udp_recv(socket->pcb.udp, NULL, NULL);
            udp_remove(socket->pcb.udp);
            break;
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
            raw_recv(socket->pcb.raw, NULL, NULL);
            raw_remove(socket->pcb.raw);
            break;
        #endif
    }

    socket->pcb.tcp = NULL;
    socket->state = _ERR_BADF;
    MICROPY_PY_LWIP_EXIT
}

void common_hal_socketpool_socket_close(socketpool_socket_obj_t *socket) {
    socketpool_socket_close(socket);
}

void common_hal_socketpool_socket_connect(socketpool_socket_obj_t *socket,
    const char *host, size_t hostlen, uint32_t port) {

    if (socket->pcb.tcp == NULL) {
        mp_raise_OSError(MP_EBADF);
    }

    // get address
    ip_addr_t dest;
    socketpool_resolve_host_raise(socket->pool, host, &dest);

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
            if (socket->timeout != (unsigned)-1) {
                for (mp_uint_t retries = socket->timeout / 100; retries--;) {
                    mp_hal_delay_ms(100);
                    if (socket->state != STATE_CONNECTING) {
                        break;
                    }
                }
                if (socket->state == STATE_CONNECTING) {
                    mp_raise_OSError(MP_EINPROGRESS);
                }
            } else {
                while (socket->state == STATE_CONNECTING) {
                    poll_sockets();
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
}

bool common_hal_socketpool_socket_get_closed(socketpool_socket_obj_t *socket) {
    return !socket->pcb.tcp;
}

bool common_hal_socketpool_socket_get_connected(socketpool_socket_obj_t *socket) {
    return socket->state == STATE_CONNECTED;
}

bool common_hal_socketpool_socket_listen(socketpool_socket_obj_t *socket, int backlog) {
    if (socket->type != MOD_NETWORK_SOCK_STREAM) {
        mp_raise_OSError(MP_EOPNOTSUPP);
    }

    struct tcp_pcb *new_pcb = tcp_listen_with_backlog(socket->pcb.tcp, (u8_t)backlog);
    if (new_pcb == NULL) {
        mp_raise_OSError(MP_ENOMEM);
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

mp_uint_t common_hal_socketpool_socket_recvfrom_into(socketpool_socket_obj_t *socket,
    uint8_t *buf, uint32_t len, uint8_t *ip, uint32_t *port) {
    int _errno;

    mp_uint_t ret = 0;
    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM: {
            memcpy(ip, &socket->peer, sizeof(socket->peer));
            *port = (mp_uint_t)socket->peer_port;
            ret = lwip_tcp_receive(socket, (byte *)buf, len, &_errno);
            break;
        }
        case SOCKETPOOL_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_receive(socket, (byte *)buf, len, ip, port, &_errno);
            break;
    }
    if (ret == (unsigned)-1) {
        mp_raise_OSError(_errno);
    }

    return ret;
}

int socketpool_socket_recv_into(socketpool_socket_obj_t *socket,
    const uint8_t *buf, uint32_t len) {
    mp_uint_t ret = 0;
    int _errno = 0;
    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM: {
            ret = lwip_tcp_receive(socket, (byte *)buf, len, &_errno);
            break;
        }
        case SOCKETPOOL_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_receive(socket, (byte *)buf, len, NULL, NULL, &_errno);
            break;
    }
    if (ret == (unsigned)-1) {
        mp_raise_OSError(_errno);
    }
    return ret;
}

mp_uint_t common_hal_socketpool_socket_recv_into(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int received = socketpool_socket_recv_into(self, buf, len);
    if (received < 0) {
        mp_raise_OSError(-received);
    }
    return received;
}

int socketpool_socket_send(socketpool_socket_obj_t *socket, const uint8_t *buf, uint32_t len) {
    mp_uint_t ret = 0;
    int _errno = 0;
    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, buf, len, &_errno);
            break;
        }
        case SOCKETPOOL_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_send(socket, buf, len, NULL, 0, &_errno);
            break;
    }
    if (ret == (unsigned)-1) {
        return -_errno;
    }
    return ret;
}

mp_uint_t common_hal_socketpool_socket_send(socketpool_socket_obj_t *self, const uint8_t *buf, uint32_t len) {
    int sent = socketpool_socket_send(self, buf, len);

    if (sent < 0) {
        mp_raise_OSError(-sent);
    }
    return sent;
}

mp_uint_t common_hal_socketpool_socket_sendto(socketpool_socket_obj_t *socket,
    const char *host, size_t hostlen, uint32_t port, const uint8_t *buf, uint32_t len) {
    int _errno;
    ip_addr_t ip;
    socketpool_resolve_host_raise(socket->pool, host, &ip);

    mp_uint_t ret = 0;
    switch (socket->type) {
        case SOCKETPOOL_SOCK_STREAM: {
            ret = lwip_tcp_send(socket, buf, len, &_errno);
            break;
        }
        case SOCKETPOOL_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
        #endif
            ret = lwip_raw_udp_send(socket, buf, len, &ip, port, &_errno);
            break;
    }
    if (ret == (unsigned)-1) {
        mp_raise_OSError(_errno);
    }

    return ret;
}

void common_hal_socketpool_socket_settimeout(socketpool_socket_obj_t *self, uint32_t timeout_ms) {
    self->timeout = timeout_ms;
}

int common_hal_socketpool_socket_setsockopt(socketpool_socket_obj_t *self, int level, int optname, const void *value, size_t optlen) {
    if (level == SOCKETPOOL_IPPROTO_TCP && optname == SOCKETPOOL_TCP_NODELAY) {
        int one = 1;
        bool enable = optlen == sizeof(&one) && memcmp(value, &one, optlen);
        if (enable) {
            tcp_set_flags(self->pcb.tcp, TF_NODELAY);
        } else {
            tcp_clear_flags(self->pcb.tcp, TF_NODELAY);
        }
        return 0;
    }
    return -MP_EOPNOTSUPP;
}

bool common_hal_socketpool_readable(socketpool_socket_obj_t *self) {

    MICROPY_PY_LWIP_ENTER;

    bool result = self->incoming.pbuf != NULL;

    if (self->state == STATE_PEER_CLOSED) {
        result = true;
    }

    if (self->type == SOCKETPOOL_SOCK_STREAM && self->pcb.tcp->state == LISTEN) {
        struct tcp_pcb *volatile *incoming_connection = &lwip_socket_incoming_array(self)[self->incoming.connection.iget];
        result = (incoming_connection != NULL);
    }

    MICROPY_PY_LWIP_EXIT;

    return result;
}

bool common_hal_socketpool_writable(socketpool_socket_obj_t *self) {
    bool result = false;

    MICROPY_PY_LWIP_ENTER;

    switch (self->type) {
        case SOCKETPOOL_SOCK_STREAM: {
            result = tcp_sndbuf(self->pcb.tcp) != 0;
            break;
        }
        case SOCKETPOOL_SOCK_DGRAM:
        #if MICROPY_PY_LWIP_SOCK_RAW
        case SOCKETPOOL_SOCK_RAW:
        #endif
            result = true;
            break;
    }

    MICROPY_PY_LWIP_EXIT;

    return result;
}

void socketpool_socket_move(socketpool_socket_obj_t *self, socketpool_socket_obj_t *sock) {
    *sock = *self;
    self->state = _ERR_BADF;

    // Reregister the callbacks with the new socket copy.
    MICROPY_PY_LWIP_ENTER;

    tcp_arg(self->pcb.tcp, NULL);
    tcp_err(self->pcb.tcp, NULL);
    tcp_recv(self->pcb.tcp, NULL);

    self->pcb.tcp = NULL;

    tcp_arg(sock->pcb.tcp, (void *)sock);
    tcp_err(sock->pcb.tcp, _lwip_tcp_error);
    tcp_recv(sock->pcb.tcp, _lwip_tcp_recv);

    MICROPY_PY_LWIP_EXIT;
}

void socketpool_socket_reset(socketpool_socket_obj_t *self) {
    if (self->base.type == &socketpool_socket_type) {
        return;
    }
    self->base.type = &socketpool_socket_type;
    self->pcb.tcp = NULL;
    self->state = _ERR_BADF;
}
