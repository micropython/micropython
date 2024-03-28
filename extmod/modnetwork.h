/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_MODNETWORK_H
#define MICROPY_INCLUDED_MODNETWORK_H

#define MOD_NETWORK_IPADDR_BUF_SIZE (4)

#define MOD_NETWORK_AF_INET (2)
#define MOD_NETWORK_AF_INET6 (10)

#define MOD_NETWORK_SOCK_STREAM (1)
#define MOD_NETWORK_SOCK_DGRAM (2)
#define MOD_NETWORK_SOCK_RAW (3)

#define MOD_NETWORK_STA_IF (0)
#define MOD_NETWORK_AP_IF (1)

// Socket level option.
#define MOD_NETWORK_SOL_SOCKET      (0x0FFF)

// Common option flags per-socket.
#define MOD_NETWORK_SO_REUSEADDR    (0x0004)
#define MOD_NETWORK_SO_BROADCAST    (0x0020)
#define MOD_NETWORK_SO_KEEPALIVE    (0x0008)
#define MOD_NETWORK_SO_SNDTIMEO     (0x1005)
#define MOD_NETWORK_SO_RCVTIMEO     (0x1006)

#define MOD_NETWORK_SS_NEW          (0)
#define MOD_NETWORK_SS_LISTENING    (1)
#define MOD_NETWORK_SS_CONNECTED    (2)
#define MOD_NETWORK_SS_CLOSED       (3)

extern char mod_network_country_code[2];

#ifndef MICROPY_PY_NETWORK_HOSTNAME_MAX_LEN
// Doesn't include the null terminator.
#define MICROPY_PY_NETWORK_HOSTNAME_MAX_LEN (32)
#endif

// This is a null-terminated string.
extern char mod_network_hostname_data[MICROPY_PY_NETWORK_HOSTNAME_MAX_LEN + 1];

// To support backwards-compatible (esp32, esp8266, cyw43)
// `if.config(hostname=...)` to forward directly to the implementation of
// `network.hostname(...)`.
mp_obj_t mod_network_hostname(size_t n_args, const mp_obj_t *args);

#if MICROPY_PY_LWIP

#include "lwip/init.h"

struct netif;
void mod_network_lwip_init(void);
void mod_network_lwip_poll_wrapper(uint32_t ticks_ms);
mp_obj_t mod_network_nic_ifconfig(struct netif *netif, size_t n_args, const mp_obj_t *args);
#if LWIP_VERSION_MAJOR >= 2
mp_obj_t mod_network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
mp_obj_t mod_network_nic_ipconfig(struct netif *netif, size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
extern int mp_mod_network_prefer_dns_use_ip_version;
#endif
#elif defined(MICROPY_PORT_NETWORK_INTERFACES)

#if MICROPY_PY_NETWORK_NINAW10
mp_obj_t network_ninaw10_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs);
#endif

struct _mod_network_socket_obj_t;

typedef struct _mod_network_nic_protocol_t {
    // API for non-socket operations
    int (*gethostbyname)(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *ip_out);
    void (*deinit)(void);

    // API for socket operations; return -1 on error
    int (*socket)(struct _mod_network_socket_obj_t *socket, int *_errno);
    void (*close)(struct _mod_network_socket_obj_t *socket);
    int (*bind)(struct _mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
    int (*listen)(struct _mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno);
    int (*accept)(struct _mod_network_socket_obj_t *socket, struct _mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno);
    int (*connect)(struct _mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
    mp_uint_t (*send)(struct _mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno);
    mp_uint_t (*recv)(struct _mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno);
    mp_uint_t (*sendto)(struct _mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno);
    mp_uint_t (*recvfrom)(struct _mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno);
    int (*setsockopt)(struct _mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno);
    int (*settimeout)(struct _mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno);
    int (*ioctl)(struct _mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno);
} mod_network_nic_protocol_t;

typedef struct _mod_network_socket_obj_t {
    mp_obj_base_t base;
    mp_obj_t nic;
    mod_network_nic_protocol_t *nic_protocol;
    uint32_t domain : 5;
    uint32_t type   : 5;
    uint32_t proto  : 5;
    uint32_t bound  : 1;
    int32_t fileno  : 16;
    int32_t timeout;
    mp_obj_t callback;
    int32_t state   : 8;
    #if MICROPY_PY_SOCKET_EXTENDED_STATE
    // Extended socket state for NICs/ports that need it.
    void *_private;
    #endif
} mod_network_socket_obj_t;

#endif // MICROPY_PY_LWIP / MICROPY_PORT_NETWORK_INTERFACES

#ifdef MICROPY_PORT_NETWORK_INTERFACES
void mod_network_init(void);
void mod_network_deinit(void);
void mod_network_register_nic(mp_obj_t nic);
mp_obj_t mod_network_find_nic(const uint8_t *ip);
#endif

#endif // MICROPY_INCLUDED_MODNETWORK_H
