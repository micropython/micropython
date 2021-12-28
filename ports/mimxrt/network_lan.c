/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
#include "py/mphal.h"
#include "extmod/modnetwork.h"

#if defined(MICROPY_HW_ETH_MDC)

#include "fsl_phy.h"
#include "eth.h"
#include "hal/phy/device/phyksz8081/fsl_phyksz8081.h"
#include "hal/phy/device/phydp83825/fsl_phydp83825.h"
#include "hal/phy/device/phydp83848/fsl_phydp83848.h"
#include "hal/phy/device/phylan8720/fsl_phylan8720.h"

#include "lwip/netif.h"

#ifndef ENET_TX_CLK_OUTPUT
#define ENET_TX_CLK_OUTPUT true
#endif

typedef struct _network_lan_obj_t {
    mp_obj_base_t base;
    eth_t *eth;
} network_lan_obj_t;

STATIC const network_lan_obj_t network_lan_eth0 = { { &network_lan_type }, &eth_instance };

STATIC void network_lan_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct netif *netif = eth_netif(self->eth);
    int status = eth_link_status(self->eth);
    mp_printf(print, "<ETH %u %u.%u.%u.%u>",
        status,
        netif->ip_addr.addr & 0xff,
        netif->ip_addr.addr >> 8 & 0xff,
        netif->ip_addr.addr >> 16 & 0xff,
        netif->ip_addr.addr >> 24
        );
}

STATIC mp_obj_t network_lan_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_id, ARG_phy_type, ARG_phy_addr, ARG_phy_clock};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_id, MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phy_type, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_phy_addr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = ENET_PHY_ADDRESS} },
        { MP_QSTR_phy_clock, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = ENET_TX_CLK_OUTPUT} },
    };
    // Parse args.
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const phy_operations_t *phy_ops = &ENET_PHY_OPS;

    // Select PHY driver
    int phy_type = args[ARG_phy_type].u_int;
    if (phy_type != -1) {
        if (phy_type == PHY_KSZ8081) {
            phy_ops = &phyksz8081_ops;
        } else if (phy_type == PHY_DP83825) {
            phy_ops = &phydp83825_ops;
        } else if (phy_type == PHY_DP83848) {
            phy_ops = &phydp83848_ops;
        } else if (phy_type == PHY_LAN8720) {
            phy_ops = &phylan8720_ops;
        } else {
            mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid value %d for phy"), phy_type);
        }
    }
    int phy_addr = args[ARG_phy_addr].u_int;

    bool phy_clock = args[ARG_phy_clock].u_int;

    // Prepare for two ETH interfaces.
    const network_lan_obj_t *self;
    int mac_id = args[ARG_id].u_int;
    if (mac_id == 0) {
        self = &network_lan_eth0;
        mac_id = MP_HAL_MAC_ETH0;
    } else if (mac_id == 1) {
        self = &network_lan_eth0;
        mac_id = MP_HAL_MAC_ETH1;
    } else {
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("Invalid LAN interface %d"), mac_id);
    }

    eth_init(self->eth, mac_id, phy_ops, phy_addr, phy_clock);
    // register with network module
    mod_network_register_nic((mp_obj_t *)self);
    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t network_lan_active(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(eth_link_status(self->eth));
    } else {
        int ret;
        if (mp_obj_is_true(args[1])) {
            ret = eth_start(self->eth);
        } else {
            ret = eth_stop(self->eth);
        }
        if (ret < 0) {
            mp_raise_OSError(-ret);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_active_obj, 1, 2, network_lan_active);

STATIC mp_obj_t network_lan_isconnected(mp_obj_t self_in) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(eth_link_status(self->eth) == 3);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(network_lan_isconnected_obj, network_lan_isconnected);

STATIC mp_obj_t network_lan_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(eth_netif(self->eth), n_args - 1, args + 1);
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_ifconfig_obj, 1, 2, network_lan_ifconfig);

STATIC mp_obj_t network_lan_status(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (n_args == 1) {
        // No arguments: return link status
        return MP_OBJ_NEW_SMALL_INT(eth_link_status(self->eth));
    }

    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_status_obj, 1, 2, network_lan_status);

STATIC mp_obj_t network_lan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_mac: {
                return mp_obj_new_bytes(&eth_netif(self->eth)->hwaddr[0], 6);
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_trace: {
                        eth_set_trace(self->eth, mp_obj_get_int(e->value));
                        break;
                    }
                    case MP_QSTR_low_power: {
                        eth_low_power_mode(self->eth, mp_obj_get_int(e->value));
                        break;
                    }
                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
                }
            }
        }

        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(network_lan_config_obj, 1, network_lan_config);

STATIC const mp_rom_map_elem_t network_lan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&network_lan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_lan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_lan_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_lan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_lan_config_obj) },

    { MP_ROM_QSTR(MP_QSTR_PHY_KSZ8081), MP_ROM_INT(PHY_KSZ8081) },
    { MP_ROM_QSTR(MP_QSTR_PHY_DP83825), MP_ROM_INT(PHY_DP83825) },
    { MP_ROM_QSTR(MP_QSTR_PHY_DP83848), MP_ROM_INT(PHY_DP83848) },
    { MP_ROM_QSTR(MP_QSTR_PHY_LAN8720), MP_ROM_INT(PHY_LAN8720) },
    { MP_ROM_QSTR(MP_QSTR_IN), MP_ROM_INT(PHY_TX_CLK_IN) },
    { MP_ROM_QSTR(MP_QSTR_OUT), MP_ROM_INT(PHY_TX_CLK_OUT) },
};
STATIC MP_DEFINE_CONST_DICT(network_lan_locals_dict, network_lan_locals_dict_table);

const mp_obj_type_t network_lan_type = {
    { &mp_type_type },
    .name = MP_QSTR_LAN,
    .print = network_lan_print,
    .make_new = network_lan_make_new,
    .locals_dict = (mp_obj_dict_t *)&network_lan_locals_dict,
};

#endif // defined(MICROPY_HW_ETH_MDC)
