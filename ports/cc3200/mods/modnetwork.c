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

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "shared/netutils/netutils.h"
#include "modnetwork.h"
#include "serverstask.h"
#include "simplelink.h"


/******************************************************************************
 DEFINE TYPES
 ******************************************************************************/
typedef struct {
    mp_obj_base_t base;
} network_server_obj_t;

/******************************************************************************
 DECLARE PRIVATE DATA
 ******************************************************************************/
static network_server_obj_t network_server_obj;
static const mp_obj_type_t network_server_type;

/// \module network - network configuration
///
/// This module provides network drivers and server configuration.

void mod_network_init0(void) {
}

static mp_obj_t mod_network_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    unsigned char len = sizeof(SlNetCfgIpV4Args_t);
    unsigned char dhcpIsOn;
    SlNetCfgIpV4Args_t ipV4;
    sl_NetCfgGet(SL_IPV4_STA_P2P_CL_GET_INFO, &dhcpIsOn, &len, (uint8_t *)&ipV4);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }
        switch (mp_obj_str_get_qstr(args[0])) {
            case MP_QSTR_dns: {
                return netutils_format_ipv4_addr((uint8_t *)&ipV4.ipV4DnsServer, NETUTILS_LITTLE);
            }
            default: {
                mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                break;
            }
        }
    } else {
        // Set config value(s)
        if (n_args != 0) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_dns: {
                        netutils_parse_ipv4_addr(e->value, (uint8_t *)&ipV4.ipV4DnsServer, NETUTILS_LITTLE);
                        sl_NetCfgSet(SL_IPV4_STA_P2P_CL_STATIC_ENABLE, IPCONFIG_MODE_ENABLE_IPV4, sizeof(SlNetCfgIpV4Args_t), (_u8 *)&ipV4);
                        break;
                    }
                    default: {
                        mp_raise_ValueError(MP_ERROR_TEXT("unexpected key"));
                        break;
                    }
                }
            }
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(mod_network_ipconfig_obj, 0, mod_network_ipconfig);

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
static mp_obj_t network_server_init_helper(mp_obj_t self, const mp_arg_val_t *args) {
    const char *user = SERVERS_DEF_USER;
    const char *pass = SERVERS_DEF_PASS;
    if (args[0].u_obj != MP_OBJ_NULL) {
        mp_obj_t *login;
        mp_obj_get_array_fixed_n(args[0].u_obj, 2, &login);
        user = mp_obj_str_get_str(login[0]);
        pass = mp_obj_str_get_str(login[1]);
    }

    uint32_t timeout = SERVERS_DEF_TIMEOUT_MS / 1000;
    if (args[1].u_obj != MP_OBJ_NULL) {
        timeout = mp_obj_get_int(args[1].u_obj);
    }

    // configure the new login
    servers_set_login ((char *)user, (char *)pass);

    // configure the timeout
    servers_set_timeout(timeout * 1000);

    // start the servers
    servers_start();

    return mp_const_none;
}

static const mp_arg_t network_server_args[] = {
    { MP_QSTR_id,                            MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_login,        MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
};
static mp_obj_t network_server_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    // parse args
    mp_map_t kw_args;
    mp_map_init_fixed_table(&kw_args, n_kw, all_args + n_args);
    mp_arg_val_t args[MP_ARRAY_SIZE(network_server_args)];
    mp_arg_parse_all(n_args, all_args, &kw_args, MP_ARRAY_SIZE(args), network_server_args, args);

    // check the server id
    if (args[0].u_obj != MP_OBJ_NULL) {
        if (mp_obj_get_int(args[0].u_obj) != 0) {
            mp_raise_OSError(MP_ENODEV);
        }
    }

    // setup the object and initialize it
    network_server_obj_t *self = &network_server_obj;
    self->base.type = &network_server_type;
    network_server_init_helper(self, &args[1]);

    return (mp_obj_t)self;
}

static mp_obj_t network_server_init(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(network_server_args) - 1];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(args), &network_server_args[1], args);
    return network_server_init_helper(pos_args[0], args);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_server_init_obj, 1, network_server_init);

// timeout value given in seconds
static mp_obj_t network_server_timeout(size_t n_args, const mp_obj_t *args) {
    if (n_args > 1) {
        uint32_t timeout = mp_obj_get_int(args[1]);
        servers_set_timeout(timeout * 1000);
        return mp_const_none;
    } else {
        // get
        return mp_obj_new_int(servers_get_timeout() / 1000);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_server_timeout_obj, 1, 2, network_server_timeout);

static mp_obj_t network_server_running(mp_obj_t self_in) {
    // get
    return mp_obj_new_bool(servers_are_enabled());
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_server_running_obj, network_server_running);

static mp_obj_t network_server_deinit(mp_obj_t self_in) {
    // simply stop the servers
    servers_stop();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_server_deinit_obj, network_server_deinit);
#endif

static const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),            MP_ROM_PTR(&mod_network_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_WLAN),                MP_ROM_PTR(&mod_network_nic_type_wlan) },

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
    { MP_ROM_QSTR(MP_QSTR_Server),              MP_ROM_PTR(&network_server_type) },
#endif
};

static MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_network_globals,
};

MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network);

#if (MICROPY_PORT_HAS_TELNET || MICROPY_PORT_HAS_FTP)
static const mp_rom_map_elem_t network_server_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),                MP_ROM_PTR(&network_server_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),              MP_ROM_PTR(&network_server_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout),             MP_ROM_PTR(&network_server_timeout_obj) },
    { MP_ROM_QSTR(MP_QSTR_isrunning),           MP_ROM_PTR(&network_server_running_obj) },
};

static MP_DEFINE_CONST_DICT(network_server_locals_dict, network_server_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    network_server_type,
    MP_QSTR_Server,
    MP_TYPE_FLAG_NONE,
    make_new, network_server_make_new,
    locals_dict, &network_server_locals_dict
    );
#endif
