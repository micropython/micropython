/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/nlr.h"
#include "netutils.h"

// Takes an array with a raw IPv4 address and returns something like '192.168.0.1'.
mp_obj_t netutils_format_ipv4_addr(uint8_t *ip, netutils_endian_t endian) {
    char ip_str[16];
    mp_uint_t ip_len;
    if (endian == NETUTILS_LITTLE) {
        ip_len = snprintf(ip_str, 16, "%u.%u.%u.%u", ip[3], ip[2], ip[1], ip[0]);
    } else {
        ip_len = snprintf(ip_str, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    }
    return mp_obj_new_str(ip_str, ip_len, false);
}

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('192.168.0.1', 8080).
mp_obj_t netutils_format_inet_addr(uint8_t *ip, mp_uint_t port, netutils_endian_t endian) {
    mp_obj_t tuple[2] = {
        tuple[0] = netutils_format_ipv4_addr(ip, endian),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}

void netutils_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian) {
    mp_uint_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_in, &addr_len);
    if (addr_len == 0) {
        // special case of no address given
        memset(out_ip, 0, NETUTILS_IPV4ADDR_BUFSIZE);
        return;
    }
    const char *s = addr_str;
    const char *s_top = addr_str + addr_len;
    for (mp_uint_t i = 3 ; ; i--) {
        mp_uint_t val = 0;
        for (; s < s_top && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        if (endian == NETUTILS_LITTLE) {
            out_ip[i] = val;
        } else {
            out_ip[NETUTILS_IPV4ADDR_BUFSIZE - 1 - i] = val;
        }
        if (i == 0 && s == s_top) {
            return;
        } else if (i > 0 && s < s_top && *s == '.') {
            s++;
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid arguments"));
        }
    }
}

// Takes an address of the form ('192.168.0.1', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
mp_uint_t netutils_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    netutils_parse_ipv4_addr(addr_items[0], out_ip, endian);
    return mp_obj_get_int(addr_items[1]);
}


// Takes an array with a raw IPv6 address and returns something like '2001:db8::abcd:ef01:2345'.
mp_obj_t netutils_format_ipv6_addr(uint8_t *ip, netutils_endian_t endian) {
    char ip_str[40];
    mp_uint_t ip_len;
    if (endian == NETUTILS_LITTLE) {
        ip_len = snprintf(ip_str, 40, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                          ip[15], ip[14], ip[13], ip[12],
                          ip[11], ip[10], ip[9],  ip[8],
                          ip[7],  ip[6],  ip[5],  ip[4],
                          ip[3],  ip[2],  ip[1],  ip[0]);
    } else {
        ip_len = snprintf(ip_str, 40, "%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
                          ip[0],  ip[1],  ip[2],  ip[3],
                          ip[4],  ip[5],  ip[6],  ip[7],
                          ip[8],  ip[9],  ip[10], ip[11],
                          ip[12], ip[13], ip[14], ip[15]);
    }
    return mp_obj_new_str(ip_str, ip_len, false);
}

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('2001:db8::abcd:ef01:2345', 8080).
mp_obj_t netutils_format_inet6_addr(uint8_t *ip, mp_uint_t port, netutils_endian_t endian) {
    mp_obj_t tuple[2] = {
        tuple[0] = netutils_format_ipv6_addr(ip, endian),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}

void netutils_parse_ipv6_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian) {
    mp_uint_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_in, &addr_len);
    if (addr_len == 0) {
        // special case of no address given
        memset(out_ip, 0, NETUTILS_IPV6ADDR_BUFSIZE);
        return;
    }

    const char *s = addr_str;
    const char *s_top = addr_str + addr_len;

    for (uint8_t i = 0; i <= NETUTILS_IPV6ADDR_BUFSIZE; i += 2) {
        uint16_t val = 0;
        for (; s < s_top && *s != ':'; s++) {
            if ((*s >= 'a' && *s <= 'f')) {
                val = val * 16 + (*s - 'a' + 10);
            } else if (*s >= 'A' && *s <= 'F') {
                val = val * 16 + (*s - 'A' + 10);
            } else {
                val = val * 16 + (*s - '0');
            }
        }

//      if (endian == NETUTILS_LITTLE) {
//          // not supported
//      } else
        {
            out_ip[i] = (val >> 8);
            out_ip[i + 1] = (val & 0xFF);
        }

        if (s == s_top) {
            return;
        } else if (s < s_top && *s == ':') {
            s++;
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid arguments"));
        }
    }
}

// Takes an address of the form ('2001:db8::abcd:ef01:2345', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
mp_uint_t netutils_parse_inet6_addr(mp_obj_t addr_in, uint8_t *out_ip, netutils_endian_t endian) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    netutils_parse_ipv6_addr(addr_items[0], out_ip, endian);
    return mp_obj_get_int(addr_items[1]);

}
