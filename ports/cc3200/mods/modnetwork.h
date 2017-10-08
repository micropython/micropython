/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_CC3200_MODS_MODNETWORK_H
#define MICROPY_INCLUDED_CC3200_MODS_MODNETWORK_H

/******************************************************************************
 DEFINE CONSTANTS
 ******************************************************************************/
#define MOD_NETWORK_IPV4ADDR_BUF_SIZE             (4)

/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct _mod_network_nic_type_t {
    mp_obj_type_t base;
} mod_network_nic_type_t;

typedef struct _mod_network_socket_base_t {
    union {
        struct {
            // this order is important so that fileno gets > 0 once
            // the socket descriptor is assigned after being created.
            uint8_t domain;
            int8_t fileno;
            uint8_t type;
            uint8_t proto;
        } u_param;
        int16_t sd;
    };
    uint32_t timeout_ms; // 0 for no timeout
    bool cert_req;
} mod_network_socket_base_t;

typedef struct _mod_network_socket_obj_t {
    mp_obj_base_t base;
    mod_network_socket_base_t sock_base;
} mod_network_socket_obj_t;

/******************************************************************************
 EXPORTED DATA
 ******************************************************************************/
extern const mod_network_nic_type_t mod_network_nic_type_wlan;

/******************************************************************************
 DECLARE FUNCTIONS
 ******************************************************************************/
void mod_network_init0(void);

#endif // MICROPY_INCLUDED_CC3200_MODS_MODNETWORK_H
