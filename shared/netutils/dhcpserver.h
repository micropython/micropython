/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#ifndef MICROPY_INCLUDED_LIB_NETUTILS_DHCPSERVER_H
#define MICROPY_INCLUDED_LIB_NETUTILS_DHCPSERVER_H

#include "lwip/ip_addr.h"

#define DHCPS_BASE_IP (16)
#define DHCPS_MAX_IP (8)

typedef struct _dhcp_server_lease_t {
    uint8_t mac[6];
    uint16_t expiry;
} dhcp_server_lease_t;

typedef struct _dhcp_server_t {
    ip_addr_t ip;
    ip_addr_t nm;
    dhcp_server_lease_t lease[DHCPS_MAX_IP];
    struct udp_pcb *udp;
} dhcp_server_t;

void dhcp_server_init(dhcp_server_t *d, ip_addr_t *ip, ip_addr_t *nm);
void dhcp_server_deinit(dhcp_server_t *d);

#endif // MICROPY_INCLUDED_LIB_NETUTILS_DHCPSERVER_H
