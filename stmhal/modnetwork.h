/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#define MOD_NETWORK_IPADDR_BUF_SIZE (4)

#define MOD_NETWORK_AF_INET (2)
#define MOD_NETWORK_AF_INET6 (10)

#define MOD_NETWORK_SOCK_STREAM (1)
#define MOD_NETWORK_SOCK_DGRAM (2)
#define MOD_NETWORK_SOCK_RAW (3)

typedef struct _mod_network_nic_type_t {
    mp_obj_type_t base;

    // API for a generic NIC
    mp_obj_t (*socket)(mp_obj_t nic, int domain, int type, int fileno, int *_errno);
    int (*gethostbyname)(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *ip_out);
} mod_network_nic_type_t;

extern struct _mp_obj_list_t mod_network_nic_list;
extern const mod_network_nic_type_t mod_network_nic_type_wiznet5k;
extern const mod_network_nic_type_t mod_network_nic_type_cc3k;

void mod_network_init(void);
void mod_network_register_nic(mp_obj_t nic);

void mod_network_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip);
mp_uint_t mod_network_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip);
mp_obj_t mod_network_format_ipv4_addr(uint8_t *ip);
mp_obj_t mod_network_format_inet_addr(uint8_t *ip, mp_uint_t port);
