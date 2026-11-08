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

    // Report which stage failed - "failed to init ethernet" on its own gives the
    // user nothing to act on, and the usual cause is simply that no PHY answered
    // on the MDIO bus (board without a PHY, or PHY held in reset).
    int rc = eth_init((eth_t *)&eth_instance_for_lan, (uint32_t)args[ARG_phy_addr].u_int);
    if (rc != 0) {
        switch (rc) {
            case -3:
                mp_raise_OSError(MP_ENODEV);      // no PHY answered on MDIO
            case -4:
                mp_raise_msg(&mp_type_OSError,
                    MP_ERROR_TEXT("PHY not at MDIO address 1"));
            case -2:
                mp_raise_msg(&mp_type_OSError,
                    MP_ERROR_TEXT("PHY auto-negotiation failed"));
            default:
                mp_raise_msg(&mp_type_OSError,
                    MP_ERROR_TEXT("ENET MAC init failed"));
        }
    }
    // Register the interface with lwIP.  eth.c provides eth_netif_add() for this
    // and nothing ever called it, so the netif stayed all-zero: netif->input was
    // NULL, and the first received frame (any broadcast, e.g. ARP) made the
    // ENET interrupt dereference it and hard-fault.  It also means lwIP had no
    // route, which is why connect() reported EHOSTUNREACH.
    if (eth_netif_add((eth_t *)&eth_instance_for_lan) != ERR_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("netif_add failed"));
    }

    return MP_OBJ_FROM_PTR(&network_lan_eth0);
}

static mp_obj_t network_lan_active(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    struct netif *netif = eth_netif(self->eth);
    if (n_args == 1) {
        return mp_obj_new_bool(netif_is_up(netif));
    }
    // Bring the lwIP interface up/down.  The MAC itself has no separate enable:
    // this used to be a no-op that only reported the link, so nothing ever
    // called netif_set_up() and lwIP would not transmit even with a live link.
    if (mp_obj_is_true(args[1])) {
        netif_set_link_up(netif);
        netif_set_up(netif);
    } else {
        netif_set_link_down(netif);
        netif_set_down(netif);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_active_obj, 1, 2, network_lan_active);

static mp_obj_t network_lan_isconnected(mp_obj_t self_in) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(eth_link_status(self->eth));
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_lan_isconnected_obj, network_lan_isconnected);

// ifconfig() with no argument returns (ip, netmask, gateway, dns); with an
// argument it applies one.  This used to be a hand-written getter that returned
// only the IP string and rejected any argument with TypeError, so a static
// address could not be configured from Python at all.  The shared helper is what
// ports/stm32 uses and handles both directions plus multi-argument forms.
static mp_obj_t network_lan_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(eth_netif(self->eth), n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_lan_ifconfig_obj, 1, 2, network_lan_ifconfig);

static mp_obj_t network_lan_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_lan_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(eth_netif(self->eth), n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_lan_ipconfig_obj, 1, network_lan_ipconfig);

// Direct PHY register access over MDIO: indispensable when bringing a new board
// up, because "no link" can mean a dead cable, a PHY held in reset, or a PHY that
// never finished auto-negotiation, and only the PHY itself can tell you which.
static mp_obj_t network_lan_phyreg(mp_obj_t self_in, mp_obj_t reg_in) {
    (void)self_in;
    mp_int_t reg = mp_obj_get_int(reg_in);
    if (reg < 0 || reg > 31) {
        mp_raise_ValueError(MP_ERROR_TEXT("phy register must be 0-31"));
    }
    uint16_t val = 0;
    if (enet_phy_write_read(ENET_PHY_READ, PHY_ADDRESS, (uint16_t)reg, &val) != SUCCESS) {
        mp_raise_OSError(MP_EIO);
    }
    return MP_OBJ_NEW_SMALL_INT(val);
}
static MP_DEFINE_CONST_FUN_OBJ_2(network_lan_phyreg_obj, network_lan_phyreg);

// ---- bring-up diagnostics ----
// diag() reports the facts needed to tell "the MAC/DMA path is broken" apart
// from "nothing is arriving from the PHY", which look identical from Python:
//
//   [0] edge count seen while sampling PA1 (RMII REF_CLK).
//       A 50 MHz clock gives a large fraction of the 10000 samples; a static
//       (or absent) clock gives 0.  This needs no oscilloscope.
//   [1] ENET_DMA_STAT   - bit6 = RS (a frame was received), bit8 = RPS (RX
//       process stopped), bit2 = TBU (TX queue ran dry, normal), bit10 = ET.
//   [2] ENET_DMA_CTL    - bit1 = start receive, bit13 = start transmit.
//   [3] ENET_MAC_CFG    - bit2 = RE, bit3 = TE, bit12 = loopback, bit14 = speed.
//   [4] frames lwIP asked the MAC to transmit
//   [5] frames delivered to lwIP by eth_poll()
//   [6] times the (now unused) ENET interrupt vector ran
extern volatile uint32_t eth_tx_frames;
extern volatile uint32_t eth_rx_frames;
extern volatile uint32_t eth_isr_count;

static mp_obj_t network_lan_diag(mp_obj_t self_in) {
    (void)self_in;
    uint32_t edges = 0;
    uint32_t prev = (GPIO_ISTAT(GPIOA) >> 1) & 1u;
    for (uint32_t i = 0; i < 10000; i++) {
        uint32_t cur = (GPIO_ISTAT(GPIOA) >> 1) & 1u;
        edges += (cur ^ prev);
        prev = cur;
    }
    mp_obj_t items[7] = {
        mp_obj_new_int(edges),
        mp_obj_new_int(ENET_DMA_STAT),
        mp_obj_new_int(ENET_DMA_CTL),
        mp_obj_new_int(ENET_MAC_CFG),
        mp_obj_new_int(eth_tx_frames),
        mp_obj_new_int(eth_rx_frames),
        mp_obj_new_int(eth_isr_count),
    };
    return mp_obj_new_tuple(7, items);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_lan_diag_obj, network_lan_diag);

// Put the MAC in internal loopback: transmitted frames appear on the receive
// path inside the chip, with the PHY and the cable completely out of the
// picture.  If frames are still not received in this mode, the fault is in this
// driver; if they are, everything on the MCU side works and the problem is
// external (reference clock, pin mapping or PHY wiring).
static mp_obj_t network_lan_loopback(mp_obj_t self_in, mp_obj_t on_in) {
    (void)self_in;
    if (mp_obj_is_true(on_in)) {
        ENET_MAC_CFG |= ENET_MAC_CFG_LBM;
    } else {
        ENET_MAC_CFG &= ~ENET_MAC_CFG_LBM;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(network_lan_loopback_obj, network_lan_loopback);

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
    { MP_ROM_QSTR(MP_QSTR_phyreg),      MP_ROM_PTR(&network_lan_phyreg_obj) },
    { MP_ROM_QSTR(MP_QSTR_diag),        MP_ROM_PTR(&network_lan_diag_obj) },
    { MP_ROM_QSTR(MP_QSTR_loopback),    MP_ROM_PTR(&network_lan_loopback_obj) },

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
