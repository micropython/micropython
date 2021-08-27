/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_LIB_NETUTILS_NETUTILS_H
#define MICROPY_INCLUDED_LIB_NETUTILS_NETUTILS_H

#define NETUTILS_IPV4ADDR_BUFSIZE    4

#define NETUTILS_TRACE_IS_TX        (0x0001)
#define NETUTILS_TRACE_PAYLOAD      (0x0002)
#define NETUTILS_TRACE_NEWLINE      (0x0004)

typedef enum _netutils_endian_t {
    NETUTILS_LITTLE,
    NETUTILS_BIG,
} netutils_endian_t;

// Takes an array with a raw IPv4 address and returns something like '192.168.0.1'.
mp_obj_t netutils_format_ipv4_addr(uint8_t *ip, netutils_endian_t endian);

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('192.168.0.1', 8080).
mp_obj_t netutils_format_inet_addr(uint8_t *ip, mp_uint_t port, netutils_endian_t endian);

void netutils_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian);

// Takes an address of the form ('192.168.0.1', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
mp_uint_t netutils_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian);

void netutils_ethernet_trace(const mp_print_t *print, size_t len, const uint8_t *buf, unsigned int flags);

#endif // MICROPY_INCLUDED_LIB_NETUTILS_NETUTILS_H
