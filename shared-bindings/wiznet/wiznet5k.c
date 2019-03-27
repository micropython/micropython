/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/objlist.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"

#if MICROPY_PY_WIZNET5K

#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/busio/SPI.h"

#include "shared-module/network/__init__.h"
#include "shared-module/wiznet/wiznet5k.h"

//| .. currentmodule:: wiznet
//|
//| :class:`WIZNET5K` -- wrapper for Wiznet 5500 Ethernet interface
//| ===============================================================
//|
//| .. class:: WIZNET5K(spi, cs, rst)
//|
//|   Create a new WIZNET5500 interface using the specified pins
//|
//|   :param spi: spi bus to use
//|   :param cs: pin to use for Chip Select
//|   :param rst: pin to sue for Reset
//|

STATIC mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // check arguments
    mp_arg_check_num(n_args, kw_args, 3, 3, false);

    return wiznet5k_create(args[0], args[1], args[2]);
}

//| .. attribute:: connected
//|
//|   is this device physically connected?
//|

STATIC mp_obj_t wiznet5k_connected_get_value(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(wizphy_getphylink() == PHY_LINK_ON);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_connected_get_value_obj, wiznet5k_connected_get_value);

const mp_obj_property_t wiznet5k_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&wiznet5k_connected_get_value_obj,
              (mp_obj_t)&mp_const_none_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//| .. attribute:: dhcp
//|
//|   is DHCP active on this device? (set to true to activate DHCP, false to turn it off)
//|

STATIC mp_obj_t wiznet5k_dhcp_get_value(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(wiznet5k_check_dhcp());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_dhcp_get_value_obj, wiznet5k_dhcp_get_value);

STATIC mp_obj_t wiznet5k_dhcp_set_value(mp_obj_t self_in, mp_obj_t value) {
    (void)self_in;
    if (mp_obj_is_true(value)) {
        wiznet5k_start_dhcp();
    } else {
        wiznet5k_stop_dhcp();
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_dhcp_set_value_obj, wiznet5k_dhcp_set_value);

const mp_obj_property_t wiznet5k_dhcp_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&wiznet5k_dhcp_get_value_obj,
              (mp_obj_t)&wiznet5k_dhcp_set_value_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//| .. method:: ifconfig(...)
//|
//|   Called without parameters, returns a tuple of
//|   (ip_address, subnet_mask, gateway_address, dns_server)
//|
//|   Or can be called with the same tuple to set those parameters.
//|

STATIC mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiz_NetInfo netinfo;
    ctlnetwork(CN_GET_NETINFO, &netinfo);
    if (n_args == 1) {
        // get
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr(netinfo.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.sn, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], netinfo.ip, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], netinfo.sn, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], netinfo.gw, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[3], netinfo.dns, NETUTILS_BIG);
        ctlnetwork(CN_SET_NETINFO, &netinfo);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);

STATIC const mp_rom_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wiznet5k_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_connected), MP_ROM_PTR(&wiznet5k_connected_obj) },
    { MP_ROM_QSTR(MP_QSTR_dhcp), MP_ROM_PTR(&wiznet5k_dhcp_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

const mod_network_nic_type_t mod_network_nic_type_wiznet5k = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_WIZNET5K,
        .make_new = wiznet5k_make_new,
        .locals_dict = (mp_obj_dict_t*)&wiznet5k_locals_dict,
    },
    .gethostbyname = wiznet5k_gethostbyname,
    .socket = wiznet5k_socket_socket,
    .close = wiznet5k_socket_close,
    .bind = wiznet5k_socket_bind,
    .listen = wiznet5k_socket_listen,
    .accept = wiznet5k_socket_accept,
    .connect = wiznet5k_socket_connect,
    .send = wiznet5k_socket_send,
    .recv = wiznet5k_socket_recv,
    .sendto = wiznet5k_socket_sendto,
    .recvfrom = wiznet5k_socket_recvfrom,
    .setsockopt = wiznet5k_socket_setsockopt,
    .settimeout = wiznet5k_socket_settimeout,
    .ioctl = wiznet5k_socket_ioctl,
    .timer_tick = wiznet5k_socket_timer_tick,
};

#endif // MICROPY_PY_WIZNET5K
