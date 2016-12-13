/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#include "nrf5_sdk_conf.h"

#if MICROPY_PY_BLE_6LOWPAN

#include "lwip/ip6_addr.h"
#include "lwip/udp.h"

#ifdef MAX
#undef MAX
#endif

#ifdef MIN
#undef MIN
#endif

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "netutils.h"
#include "modnetwork.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "lwip/ip6_addr.h"
#include "lwip/udp.h"

/// \moduleref network

typedef struct _ble_6lowpan_obj_t {
    mp_obj_base_t base;
} ble_6lowpan_obj_t;

STATIC ble_6lowpan_obj_t ble_6lowpan_obj;



STATIC int ble_6lowpan_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    return MP_ENOENT;
}

STATIC int ble_6lowpan_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC void ble_6lowpan_socket_close(mod_network_socket_obj_t *socket) {
}

STATIC int ble_6lowpan_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
	err_t err;
	socket->p_socket = udp_new_ip6();
	if (socket->p_socket) {


		err = udp_bind_ip6(socket->p_socket, ip, port);
		if (err != ERR_OK) {
			*_errno = err;
			udp_remove(socket->p_socket);
		}
	}

	return 0;
}

STATIC int ble_6lowpan_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int ble_6lowpan_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int ble_6lowpan_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC mp_uint_t ble_6lowpan_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC mp_uint_t ble_6lowpan_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

#include "netutils/netutils.h"

STATIC mp_uint_t ble_6lowpan_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {

	struct pbuf * lwip_buffer = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
	memcpy(lwip_buffer->payload, buf, len);

	printf("Sendto\n");

	err_t err = udp_sendto_ip6(socket->p_socket,
	                     lwip_buffer,
	                     ip,
	                     port);

	printf("dest: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
		   ip[0],  ip[1],  ip[2],  ip[3],
		   ip[4],  ip[5],  ip[6],  ip[7],
		   ip[8],  ip[9],  ip[10], ip[11],
		   ip[12], ip[13], ip[14], ip[15]);

	printf("port: %x\n", port);
	printf("len: %d\n", len);

    if (err != ERR_OK) {
	    *_errno = err;
    }

    return 0;
}

STATIC mp_uint_t ble_6lowpan_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int ble_6lowpan_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int ble_6lowpan_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int ble_6lowpan_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    *_errno = MP_EINVAL;
    return -1;
}

#include "sdkhelp.h"

/******************************************************************************/
// Micro Python bindings

/// \classmethod \constructor
/// Create and return a BLE 6LoWPAN object.
STATIC mp_obj_t ble_6lowpan_make_new(const mp_obj_type_t *type, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    //mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // init the BLE 6lowpan object
    ble_6lowpan_obj.base.type = (mp_obj_type_t*)&mod_network_nic_type_ble_6lowpan;

    // blocking call to wait for connection with peer.
    transport_init();

    // regiser NIC with network module
    mod_network_register_nic(&ble_6lowpan_obj);

    // return BLE 6LoWPAN object
    return &ble_6lowpan_obj;
}

const mod_network_nic_type_t mod_network_nic_type_ble_6lowpan = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_BLE6LOWPAN,
        .make_new = ble_6lowpan_make_new,
    },
    .gethostbyname = ble_6lowpan_gethostbyname,
    .socket = ble_6lowpan_socket_socket,
    .close = ble_6lowpan_socket_close,
    .bind = ble_6lowpan_socket_bind,
    .listen = ble_6lowpan_socket_listen,
    .accept = ble_6lowpan_socket_accept,
    .connect = ble_6lowpan_socket_connect,
    .send = ble_6lowpan_socket_send,
    .recv = ble_6lowpan_socket_recv,
    .sendto = ble_6lowpan_socket_sendto,
    .recvfrom = ble_6lowpan_socket_recvfrom,
    .setsockopt = ble_6lowpan_socket_setsockopt,
    .settimeout = ble_6lowpan_socket_settimeout,
    .ioctl = ble_6lowpan_socket_ioctl,
};

#endif
