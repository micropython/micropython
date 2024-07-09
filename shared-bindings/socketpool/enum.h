// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
