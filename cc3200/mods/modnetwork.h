/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#define MOD_NETWORK_IPV4ADDR_BUF_SIZE             (4)

// Forward declaration
struct _mod_network_socket_obj_t;

typedef struct _mod_network_nic_type_t {
    mp_obj_type_t base;

    // API for non-socket operations
    int (*gethostbyname)(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *ip_out, uint8_t family);

    // API for socket operations; return -1 on error
    int (*socket)(struct _mod_network_socket_obj_t *s, int *_errno);
    void (*close)(struct _mod_network_socket_obj_t *socket);
    int (*bind)(struct _mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno);
    int (*listen)(struct _mod_network_socket_obj_t *s, mp_int_t backlog, int *_errno);
    int (*accept)(struct _mod_network_socket_obj_t *s, struct _mod_network_socket_obj_t *s2, byte *ip, mp_uint_t *port, int *_errno);
    int (*connect)(struct _mod_network_socket_obj_t *s, byte *ip, mp_uint_t port, int *_errno);
    int (*send)(struct _mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, int *_errno);
    int (*recv)(struct _mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, int *_errno);
    int (*sendto)(struct _mod_network_socket_obj_t *s, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno);
    int (*recvfrom)(struct _mod_network_socket_obj_t *s, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno);
    int (*setsockopt)(struct _mod_network_socket_obj_t *s, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno);
    int (*settimeout)(struct _mod_network_socket_obj_t *s, mp_uint_t timeout_ms, int *_errno);
    int (*ioctl)(struct _mod_network_socket_obj_t *s, mp_uint_t request, mp_uint_t arg, int *_errno);
} mod_network_nic_type_t;

typedef struct _mod_network_socket_obj_t {
    mp_obj_base_t base;
    mp_obj_t nic;
    mod_network_nic_type_t *nic_type;
    union {
        struct {
            uint8_t domain;
            uint8_t type;
            uint8_t proto;
            int8_t fileno;
        } u_param;
        int16_t sd;
    };
    bool  closed;
} mod_network_socket_obj_t;

extern const mod_network_nic_type_t mod_network_nic_type_wlan;

void mod_network_init(void);
void mod_network_register_nic(mp_obj_t nic);
mp_obj_t mod_network_find_nic(const uint8_t *ip);

void mod_network_convert_ipv4_endianness(uint8_t *ip);
void mod_network_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip);
mp_uint_t mod_network_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip);
mp_obj_t mod_network_format_ipv4_addr(uint8_t *ip);
mp_obj_t mod_network_format_inet_addr(uint8_t *ip, mp_uint_t port);
