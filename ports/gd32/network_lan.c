// network_lan.c - network.LAN for the GD32F4xx port (lwIP over ENET).
//
// Mirrors ports/stm32/network_lan.c but talks to the smaller eth.c driver in
// this port, which wraps the GD32 standard peripheral library.

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "extmod/modnetwork.h"

#include "eth.h"

#if MICROPY_PY_NETWORK

#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "shared/netutils/netutils.h"

extern const mp_obj_type_t network_lan_type;

typedef struct _network_lan_obj_t {
    mp_obj_base_t base;
    eth_t *eth;
} network_lan_obj_t;

static eth_t eth_instance_for_lan;
static const network_lan_obj_t network_lan_eth0 = {
    { &network_lan_type }, &eth_instance_for_lan
};

// ---- helpers ----
static void network_lan_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct netif *netif = eth_netif(self->eth);
    int status = eth_link_status(self->eth);
    mp_printf(print, "<ETH %u %u.%u.%u.%u>",
        status,
        netif->ip_addr.addr & 0xff,
        netif->ip_addr.addr >> 8 & 0xff,
        netif->ip_addr.addr >> 16 & 0xff,
        netif->ip_addr.addr >> 24 & 0xff);
}

static mp_obj_t network_lan_make_new(const mp_obj_type_t *type, size_t n_args,
                                     size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_phy_addr };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_phy_addr, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (eth_init((eth_t *)&eth_instance_for_lan, (uint32_t)args[ARG_phy_addr].u_int) != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("failed to init ethernet"));
    }
    return MP_OBJ_FROM_PTR(&network_lan_eth0);
}

static mp_obj_t network_lan_active(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(eth_link_status(self->eth));
    }
    // The GD32 driver has no separate enable/disable; report the link state.
    (void)args[1];
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_active_obj, 1, 2, network_lan_active);

static mp_obj_t network_lan_isconnected(mp_obj_t self_in) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(eth_link_status(self->eth));
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_lan_isconnected_obj, network_lan_isconnected);

static mp_obj_t network_lan_ifconfig(mp_obj_t self_in) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    struct netif *netif = eth_netif(self->eth);
    return netutils_format_ipv4_addr((uint8_t *)&netif->ip_addr.addr, NETUTILS_BIG);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_lan_ifconfig_obj, network_lan_ifconfig);

static mp_obj_t network_lan_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct netif *netif = eth_netif(self->eth);
    return netutils_format_ipv4_addr((uint8_t *)&netif->ip_addr.addr, NETUTILS_BIG);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_lan_ipconfig_obj, 1, network_lan_ipconfig);

static mp_obj_t network_lan_status(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_int(eth_link_status(self->eth) ? 1 : 0);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_status_obj, 1, 2, network_lan_status);

static mp_obj_t network_lan_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct netif *netif = eth_netif(self->eth);

    for (size_t i = 0; i < kwargs->used; i++) {
        mp_map_elem_t *elem = &kwargs->table[i];
        switch (mp_obj_str_get_qstr(elem->key)) {
            case MP_QSTR_mac: {
                mp_buffer_info_t buf;
                mp_get_buffer_raise(elem->value, &buf, MP_BUFFER_READ);
                if (buf.len != 6) {
                    mp_raise_ValueError(MP_ERROR_TEXT("mac must be 6 bytes"));
                }
                eth_set_mac(self->eth, (const uint8_t *)buf.buf);
                memcpy(netif->hwaddr, buf.buf, 6);
                break;
            }
            case MP_QSTR_dhcp: {
                if (mp_obj_is_true(elem->value)) {
                    dhcp_start(netif);
                } else {
                    dhcp_stop(netif);
                }
                break;
            }
            default:
                break;
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_lan_config_obj, 1, network_lan_config);

// ---- constant helpers ----
static const mp_rom_map_elem_t network_lan_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active),      MP_ROM_PTR(&network_lan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_lan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig),    MP_ROM_PTR(&network_lan_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig),    MP_ROM_PTR(&network_lan_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_status),      MP_ROM_PTR(&network_lan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config),      MP_ROM_PTR(&network_lan_config_obj) },

    // Status values returned by status().
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE),        MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING),  MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTED),   MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(network_lan_locals_dict, network_lan_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    network_lan_type,
    MP_QSTR_LAN,
    MP_TYPE_FLAG_NONE,
    make_new, network_lan_make_new,
    print, network_lan_print,
    locals_dict, &network_lan_locals_dict
    );

#endif // MICROPY_PY_NETWORK
