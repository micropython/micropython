/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
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

#ifndef MICROPY_INCLUDED_SHARED_MODULE_WIZNET_WIZNET5K_H
#define MICROPY_INCLUDED_SHARED_MODULE_WIZNET_WIZNET5K_H

#include "ethernet/wizchip_conf.h"
#include "ethernet/socket.h"
#include "internet/dns/dns.h"
#include "internet/dhcp/dhcp.h"

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    mp_uint_t cris_state;
    busio_spi_obj_t *spi;
    digitalio_digitalinout_obj_t cs;
    digitalio_digitalinout_obj_t rst;
    uint8_t socket_used;
    int8_t dhcp_socket; // -1 for DHCP not in use
} wiznet5k_obj_t;

int wiznet5k_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip);
int wiznet5k_socket_socket(mod_network_socket_obj_t *socket, int *_errno);
void wiznet5k_socket_close(mod_network_socket_obj_t *socket);
int wiznet5k_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
int wiznet5k_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno);
int wiznet5k_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno);
int wiznet5k_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno);
mp_uint_t wiznet5k_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno);
mp_uint_t wiznet5k_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno);
mp_uint_t wiznet5k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno);
mp_uint_t wiznet5k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno);
int wiznet5k_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno);
int wiznet5k_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno);
int wiznet5k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno);
void wiznet5k_socket_timer_tick(mod_network_socket_obj_t *socket);
void wiznet5k_socket_deinit(mod_network_socket_obj_t *socket);
mp_obj_t wiznet5k_socket_disconnect(mp_obj_t self_in);
mp_obj_t wiznet5k_create(busio_spi_obj_t *spi_in, const mcu_pin_obj_t *cs_in, const mcu_pin_obj_t *rst_in);

int wiznet5k_start_dhcp(void);
int wiznet5k_stop_dhcp(void);
bool wiznet5k_check_dhcp(void);

extern const mod_network_nic_type_t mod_network_nic_type_wiznet5k;

#endif // MICROPY_INCLUDED_SHARED_MODULE_WIZNET_WIZNET5K_H
