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
#include "py/runtime.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK

#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"

#if MICROPY_PY_NETWORK_CYW43
// So that CYW43_LINK_xxx constants are available to MICROPY_PORT_NETWORK_INTERFACES.
#include "lib/cyw43-driver/src/cyw43.h"
extern const struct _mp_obj_type_t mp_network_cyw43_type;
#endif

#if MICROPY_PY_NETWORK_WIZNET5K
extern const struct _mp_obj_type_t mod_network_nic_type_wiznet5k;
#endif

#if MICROPY_PY_NETWORK_NINAW10
extern const struct _mp_obj_type_t mod_network_nic_type_nina;
#endif

#if MICROPY_PY_NETWORK_ESP_HOSTED
extern const struct _mp_obj_type_t mod_network_esp_hosted_type;
#endif

#ifdef MICROPY_PY_NETWORK_INCLUDEFILE
#include MICROPY_PY_NETWORK_INCLUDEFILE
#endif

/// \module network - network configuration
///
/// This module provides network drivers and routing configuration.

char mod_network_country_code[2] = "XX";

#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#error "MICROPY_PY_NETWORK_HOSTNAME_DEFAULT must be set in mpconfigport.h or mpconfigboard.h"
#endif

char mod_network_hostname_data[MICROPY_PY_NETWORK_HOSTNAME_MAX_LEN + 1] = MICROPY_PY_NETWORK_HOSTNAME_DEFAULT;

#ifdef MICROPY_PORT_NETWORK_INTERFACES

void mod_network_init(void) {
    mp_obj_list_init(&MP_STATE_PORT(mod_network_nic_list), 0);
}

void mod_network_deinit(void) {
    #if !MICROPY_PY_LWIP
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        const mod_network_nic_protocol_t *nic_protocol = MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(nic), protocol);
        if (nic_protocol->deinit) {
            nic_protocol->deinit();
        }
    }
    #endif
}

void mod_network_register_nic(mp_obj_t nic) {
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        if (MP_STATE_PORT(mod_network_nic_list).items[i] == nic) {
            // nic already registered
            return;
        }
    }
    // nic not registered so add to list
    mp_obj_list_append(MP_OBJ_FROM_PTR(&MP_STATE_PORT(mod_network_nic_list)), nic);
}

mp_obj_t mod_network_find_nic(const uint8_t *ip) {
    // find a NIC that is suited to given IP address
    for (mp_uint_t i = 0; i < MP_STATE_PORT(mod_network_nic_list).len; i++) {
        mp_obj_t nic = MP_STATE_PORT(mod_network_nic_list).items[i];
        // TODO check IP suitability here
        // mod_network_nic_protocol_t *nic_protocol = (mod_network_nic_protocol_t *)MP_OBJ_TYPE_GET_SLOT(mp_obj_get_type(nic), protocol);
        return nic;
    }

    mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("no available NIC"));
}

static mp_obj_t network_route(void) {
    return MP_OBJ_FROM_PTR(&MP_STATE_PORT(mod_network_nic_list));
}
static MP_DEFINE_CONST_FUN_OBJ_0(network_route_obj, network_route);

MP_REGISTER_ROOT_POINTER(mp_obj_list_t mod_network_nic_list);

#endif // MICROPY_PORT_NETWORK_INTERFACES

static mp_obj_t network_country(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_str(mod_network_country_code, 2);
    } else {
        size_t len;
        const char *str = mp_obj_str_get_data(args[0], &len);
        if (len != 2) {
            mp_raise_ValueError(NULL);
        }
        mod_network_country_code[0] = str[0];
        mod_network_country_code[1] = str[1];
        return mp_const_none;
    }
}
// TODO: Non-static to allow backwards-compatible pyb.country.
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_country_obj, 0, 1, network_country);

mp_obj_t mod_network_hostname(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_str_from_cstr(mod_network_hostname_data);
    } else {
        size_t len;
        const char *str = mp_obj_str_get_data(args[0], &len);
        if (len > MICROPY_PY_NETWORK_HOSTNAME_MAX_LEN) {
            mp_raise_ValueError(NULL);
        }
        memcpy(mod_network_hostname_data, str, len);
        mod_network_hostname_data[len] = 0;
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_network_hostname_obj, 0, 1, mod_network_hostname);

#if LWIP_VERSION_MAJOR >= 2
MP_DEFINE_CONST_FUN_OBJ_KW(mod_network_ipconfig_obj, 0, mod_network_ipconfig);
#endif
#if MICROPY_PY_NETWORK_NINAW10
MP_DEFINE_CONST_FUN_OBJ_KW(mod_network_ipconfig_obj, 0, network_ninaw10_ipconfig);
#endif

static const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR_country), MP_ROM_PTR(&mod_network_country_obj) },
    { MP_ROM_QSTR(MP_QSTR_hostname), MP_ROM_PTR(&mod_network_hostname_obj) },
    #if LWIP_VERSION_MAJOR >= 2 || MICROPY_PY_NETWORK_NINAW10
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&mod_network_ipconfig_obj) },
    #endif

    #if MICROPY_PY_NETWORK_PPP_LWIP
    { MP_ROM_QSTR(MP_QSTR_PPP), MP_ROM_PTR(&mp_network_ppp_lwip_type) },
    #endif

    // Defined per port in mpconfigport.h
    #ifdef MICROPY_PORT_NETWORK_INTERFACES
    { MP_ROM_QSTR(MP_QSTR_route), MP_ROM_PTR(&network_route_obj) },
    MICROPY_PORT_NETWORK_INTERFACES
    #endif

    #if MICROPY_PY_NETWORK_CYW43
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mp_network_cyw43_type) },
    // CYW43 status constants, currently for rp2 port only.
    // TODO move these to WIFI module for all ports.
    #if defined(PICO_PROGRAM_NAME) && defined(CYW43_LINK_DOWN)
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(CYW43_LINK_DOWN) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(CYW43_LINK_JOIN) },
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(CYW43_LINK_BADAUTH) },
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(CYW43_LINK_NONET) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(CYW43_LINK_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(CYW43_LINK_UP) },
    #endif
    #endif

    #if MICROPY_PY_NETWORK_WIZNET5K
    { MP_ROM_QSTR(MP_QSTR_WIZNET5K), MP_ROM_PTR(&mod_network_nic_type_wiznet5k) },
    #endif

    #if MICROPY_PY_NETWORK_NINAW10
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_nic_type_nina) },
    #endif

    #if MICROPY_PY_NETWORK_ESP_HOSTED
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_esp_hosted_type) },
    #endif

    // Allow a port to take mostly full control of the network module.
    #ifdef MICROPY_PY_NETWORK_MODULE_GLOBALS_INCLUDEFILE
    #include MICROPY_PY_NETWORK_MODULE_GLOBALS_INCLUDEFILE
    #else
    // Constants
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(MOD_NETWORK_STA_IF) },
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(MOD_NETWORK_AP_IF) },
    #endif
};

static MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);

#endif  // MICROPY_PY_NETWORK
