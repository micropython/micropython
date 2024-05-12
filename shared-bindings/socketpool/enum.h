/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
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

#pragma once

// Note: This file must be designed so it be included by ssl
// whether or not CIRCUITPY_SOCKETPOOL is set.
//
typedef enum {
    SOCKETPOOL_SOCK_STREAM = 1,
    SOCKETPOOL_SOCK_DGRAM = 2,
    SOCKETPOOL_SOCK_RAW = 3
} socketpool_socketpool_sock_t;

typedef enum {
    SOCKETPOOL_AF_INET = 2,
    SOCKETPOOL_AF_INET6 = 10
} socketpool_socketpool_addressfamily_t;

typedef enum {
    SOCKETPOOL_IPPROTO_IP = 0,
    SOCKETPOOL_IPPROTO_ICMP = 1,
    SOCKETPOOL_IPPROTO_TCP = 6,
    SOCKETPOOL_IPPROTO_UDP = 17,
    SOCKETPOOL_IPPROTO_IPV6 = 41,
    SOCKETPOOL_IPPROTO_RAW = 255,
} socketpool_socketpool_ipproto_t;

typedef enum {
    SOCKETPOOL_TCP_NODELAY = 1,
} socketpool_socketpool_tcpopt_t;

typedef enum {
    SOCKETPOOL_SOL_SOCKET = 0xfff,
} socketpool_socketpool_optlevel_t;

typedef enum {
    SOCKETPOOL_SO_REUSEADDR = 0x0004,
} socketpool_socketpool_socketopt_t;

typedef enum {
    SOCKETPOOL_IP_MULTICAST_TTL = 5,
} socketpool_socketpool_ipopt_t;

typedef enum {
    SOCKETPOOL_EAI_NONAME  = -2,
} socketpool_eai_t;
