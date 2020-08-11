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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKETPOOL_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKETPOOL_H

#include "common-hal/socketpool/SocketPool.h"

extern const mp_obj_type_t socketpool_socketpool_type;

// mp_obj_t common_hal_ipaddress_new_ipv4address(uint32_t value);
// void common_hal_ipaddress_ipv4address_construct(ipaddress_ipv4address_obj_t* self, uint8_t* buf, size_t len);
// mp_obj_t common_hal_ipaddress_ipv4address_get_packed(ipaddress_ipv4address_obj_t* self);
typedef enum {
    SOCKETPOOL_SOCK_STREAM,
    SOCKETPOOL_SOCK_DGRAM,
    SOCKETPOOL_SOCK_RAW
} socketpool_socketpool_sock_t;

typedef enum {
    SOCKETPOOL_AF_INET,
    SOCKETPOOL_AF_INET6
} socketpool_socketpool_af_t;

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_SOCKETPOOL_SOCKETPOOL_H
