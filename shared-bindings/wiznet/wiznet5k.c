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
#include "shared-bindings/microcontroller/Pin.h"

#include "shared-module/network/__init__.h"
#include "shared-module/wiznet/wiznet5k.h"

//| class WIZNET5K:
//|     """Wrapper for Wiznet 5500 Ethernet interface"""
//|
//|     def __init__(self, spi: busio.SPI, cs: microcontroller.Pin, rst: microcontroller.Pin, dhcp: bool = True) -> None:
//|         """Create a new WIZNET5500 interface using the specified pins
//|
//|         :param ~busio.SPI spi: spi bus to use
//|         :param ~microcontroller.Pin cs: pin to use for Chip Select
//|         :param ~microcontroller.Pin rst: pin to use for Reset (optional)
//|         :param bool dhcp: boolean flag, whether to start DHCP automatically (optional, keyword only, default True)
//|
//|         * The reset pin is optional: if supplied it is used to reset the
//|           wiznet board before initialization.
//|         * The SPI bus will be initialized appropriately by this library.
//|         * At present, the WIZNET5K object is a singleton, so only one WizNet
//|           interface is supported at a time."""
//|         ...
//|

STATIC mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_spi, ARG_cs, ARG_rst, ARG_dhcp };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_cs, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_rst, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_dhcp, MP_ARG_KW_ONLY | MP_ARG_BOOL, { .u_bool = true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    // TODO check type of ARG_spi?
    const mcu_pin_obj_t *cs = validate_obj_is_free_pin(args[ARG_cs].u_obj);
    const mcu_pin_obj_t *rst = validate_obj_is_free_pin_or_none(args[ARG_rst].u_obj);

    mp_obj_t ret = wiznet5k_create(args[ARG_spi].u_obj, cs, rst);
    if (args[ARG_dhcp].u_bool) {
        wiznet5k_start_dhcp();
    }
    return ret;
}

//|     connected: bool
//|     """(boolean, readonly) is this device physically connected?"""
//|

STATIC mp_obj_t wiznet5k_connected_get_value(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(wizphy_getphylink() == PHY_LINK_ON);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_connected_get_value_obj, wiznet5k_connected_get_value);

const mp_obj_property_t wiznet5k_connected_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&wiznet5k_connected_get_value_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     dhcp: bool
//|     """(boolean, readwrite) is DHCP active on this device?
//|
//|     * set to True to activate DHCP, False to turn it off"""
//|

STATIC mp_obj_t wiznet5k_dhcp_get_value(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(wiznet5k_check_dhcp());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_dhcp_get_value_obj, wiznet5k_dhcp_get_value);

STATIC mp_obj_t wiznet5k_dhcp_set_value(mp_obj_t self_in, mp_obj_t value) {
    (void)self_in;
    if (mp_obj_is_true(value)) {
        int ret = wiznet5k_start_dhcp();
        if (ret) {
            mp_raise_OSError(ret);
        }
    } else {
        int ret = wiznet5k_stop_dhcp();
        if (ret) {
            mp_raise_OSError(ret);
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_dhcp_set_value_obj, wiznet5k_dhcp_set_value);

const mp_obj_property_t wiznet5k_dhcp_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&wiznet5k_dhcp_get_value_obj,
              (mp_obj_t)&wiznet5k_dhcp_set_value_obj,
              MP_ROM_NONE},
};

//|     def ifconfig(self, params: Optional[Tuple[str, str, str, str]] = None) -> Optional[Tuple[str, str, str, str]]:
//|         """Called without parameters, returns a tuple of
//|         (ip_address, subnet_mask, gateway_address, dns_server)
//|
//|         Or can be called with the same tuple to set those parameters.
//|         Setting ifconfig parameters turns DHCP off, if it was on."""
//|         ...
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
        wiznet5k_stop_dhcp();
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
        .locals_dict = (mp_obj_dict_t *)&wiznet5k_locals_dict,
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
    .deinit = wiznet5k_socket_deinit,
};

#endif // MICROPY_PY_WIZNET5K
