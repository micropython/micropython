/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Andrew Leech
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

#if MICROPY_PY_NETWORK_NCM

// Include NCM configuration (MICROPY_PY_NETWORK_NCM_DHCP_SERVER, CFG_TUD_NCM_* sizes).
#include "extmod/network_usbd_ncm.h"

#ifndef NO_QSTR
#include "tusb.h"
#endif

#include "lwip/ethip6.h"
#include "lwip/init.h"
#include "lwip/timeouts.h"
#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"
#include "lwip/apps/mdns.h"

#include "extmod/modnetwork.h"
#include "shared/netutils/netutils.h"
#include "shared/netutils/dhcpserver.h"

#define error_printf(...) mp_printf(&mp_plat_print, "network_usbd_ncm: " __VA_ARGS__)

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif

const mp_obj_type_t mod_network_nic_type_ncm;

typedef struct _ncm_obj_t {
    mp_obj_base_t base;

    struct netif netif;
    bool init;

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gateway;

    #if MICROPY_PY_NETWORK_NCM_DHCP_SERVER
    dhcp_server_t dhcp_server;
    #endif
} ncm_obj_t;

// Global object holding the usb net state
static ncm_obj_t ncm_obj;

static mp_sched_node_t ncm_sched_node;
// Pending tud_network_recv_renew() flag. mp_sched_schedule_node is a
// one-slot queue; this flag is consumed once per scheduler drain.
static volatile bool ncm_recv_renew_pending;

#define IS_ACTIVE(self) ((self)->netif.flags & NETIF_FLAG_UP)

/* This default mac will be updated during init from hardware unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
uint8_t tud_network_mac_address[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};

// Derive a link-local IPv4 address (169.254.X.1) from a MAC address.
// Note: no ARP probe/announce is performed (RFC 3927 collision detection is not
// implemented). The address is deterministic; if two devices share the same MAC,
// they will get the same link-local IP and ARP conflicts will go undetected.
// Assumes mp_hal_get_mac() returns a per-device unique value.
static void generate_linklocal_ip_from_mac(const uint8_t mac_addr[6], uint8_t ip_addr[4]) {
    ip_addr[0] = 169;
    ip_addr[1] = 254;
    ip_addr[3] = 1;

    // FNV-1a hash over all 6 MAC bytes for better distribution than XOR-fold,
    // which produces frequent collisions for sequential MACs from the same OUI.
    uint32_t hash = 2166136261u;
    for (int i = 0; i < 6; i++) {
        hash ^= mac_addr[i];
        hash *= 16777619u;
    }
    uint8_t third_octet = (uint8_t)(hash ^ (hash >> 8) ^ (hash >> 16) ^ (hash >> 24));

    // Avoid .0 and .255
    if (third_octet == 0) {
        third_octet = 1;
    }
    if (third_octet == 255) {
        third_octet = 254;
    }

    ip_addr[2] = third_octet;
}

static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
    (void)netif;
    if (!tud_ready()) {
        return ERR_USE;
    }
    if (!tud_network_can_xmit(p->tot_len)) {
        // TinyUSB is busy with a previous transmit; let lwIP retry via its
        // own ARP/TCP retransmit timers rather than busy-waiting here.
        return ERR_USE;
    }
    tud_network_xmit(p, 0);
    return ERR_OK;
}

static err_t netif_init_cb(struct netif *netif) {
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->name[0] = 'u'; // lwIP short name "un" for USB Network
    netif->name[1] = 'n';
    netif->mtu = CFG_TUD_NET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    netif->state = NULL;
    netif->linkoutput = linkoutput_fn;
    netif->output = etharp_output;
    #if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
    netif->flags |= NETIF_FLAG_MLD6;
    #endif
    return ERR_OK;
}

static void ncm_init(void) {
    // Init the NCM object
    ncm_obj.base.type = (mp_obj_type_t *)&mod_network_nic_type_ncm;

    struct netif *netif = &(ncm_obj.netif);

    MICROPY_PY_LWIP_ENTER

    mp_hal_get_mac(MP_HAL_MAC_ETH0, tud_network_mac_address);
    tud_network_mac_address[0] = 0x02;

    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));

    // This USB Network essentially creates a link with two interfaces, one for the
    // host and another for the micropython/lwip end of the connection.
    // The MAC configured above is use for one end of the link, flip a bit for the
    // MAC on the other end of the link.
    netif->hwaddr[5] ^= 0x01;

    // Generate unique link-local IP address from MAC address
    uint8_t ip_bytes[4];
    generate_linklocal_ip_from_mac(tud_network_mac_address, ip_bytes);

    // Convert to network byte order and set IP configuration
    IP(ncm_obj.ipaddr).addr = PP_HTONL(((uint32_t)ip_bytes[0] << 24) | ((uint32_t)ip_bytes[1] << 16) | ((uint32_t)ip_bytes[2] << 8) | ip_bytes[3]);
    IP(ncm_obj.netmask).addr = PP_HTONL(0xFFFF0000);  // 255.255.0.0 for link-local
    IP(ncm_obj.gateway).addr = 0;  // No gateway for link-local

    netif = netif_add(
        netif,
        ip_2_ip4(&ncm_obj.ipaddr),
        ip_2_ip4(&ncm_obj.netmask),
        ip_2_ip4(&ncm_obj.gateway),
        NULL,
        netif_init_cb,
        ethernet_input
        );
    if (netif == NULL) {
        MICROPY_PY_LWIP_EXIT
        mp_raise_OSError(MP_ENOMEM);
    }
    #if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
    netif_set_ip6_autoconfig_enabled(netif, 1);
    #endif
    #if LWIP_NETIF_HOSTNAME
    netif_set_hostname(netif, mod_network_hostname_data);
    #endif
    if (netif_default == NULL) {
        netif_set_default(netif);
    }

    #if LWIP_MDNS_RESPONDER
    mdns_resp_add_netif(netif, mod_network_hostname_data);
    #endif

    #if MICROPY_PY_NETWORK_NCM_DHCP_SERVER
    dhcp_server_init(&ncm_obj.dhcp_server, &ncm_obj.ipaddr, &ncm_obj.netmask);
    // Don't advertise a default gateway -- this is a point-to-point USB link
    // and advertising a router causes the host to route all traffic over USB.
    ncm_obj.dhcp_server.send_router = false;
    #endif

    // register with network module
    mod_network_register_nic(&ncm_obj);
    ncm_obj.init = true;

    MICROPY_PY_LWIP_EXIT
}

static void _scheduled_tud_network_recv_renew(mp_sched_node_t *node) {
    // Acknowledge the pending receive renew. tud_network_recv_renew() must
    // be called after each received packet to release the TinyUSB receive
    // buffer; mp_sched_schedule_node is a single-slot queue so at most one
    // call is pending at any time.
    uint32_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    if (ncm_recv_renew_pending) {
        ncm_recv_renew_pending = false;
        MICROPY_END_ATOMIC_SECTION(atomic_state);
        tud_network_recv_renew();
    } else {
        MICROPY_END_ATOMIC_SECTION(atomic_state);
    }
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size) {
    if (!ncm_obj.init) {
        return false;
    }
    struct netif *netif = &(ncm_obj.netif);
    bool ret = true;
    if (size) {
        MICROPY_PY_LWIP_ENTER

        struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
        if (p == NULL) {
            MICROPY_PY_LWIP_EXIT
            // Return false without calling recv_renew: TinyUSB holds the buffer.
            // NCM RX will stall until the host retransmits and memory is available.
            return false;
        }

        // Copy buf to pbuf
        pbuf_take(p, src, size);

        if (netif->input(p, netif) != ERR_OK) {
            pbuf_free(p);
            ret = false;
        }

        MICROPY_PY_LWIP_EXIT
    }
    // Schedule tud_network_recv_renew() to run after this callback returns.
    // recv_renew must not be called from within the recv callback itself.
    uint32_t atomic_state = MICROPY_BEGIN_ATOMIC_SECTION();
    ncm_recv_renew_pending = true;
    MICROPY_END_ATOMIC_SECTION(atomic_state);
    mp_sched_schedule_node(&ncm_sched_node, _scheduled_tud_network_recv_renew);
    return ret;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg) {
    // copy from network stack packet pointer to dst.
    (void)arg;
    MICROPY_PY_LWIP_ENTER
    struct pbuf *p = (struct pbuf *)ref;
    uint16_t len = pbuf_copy_partial(p, dst, p->tot_len, 0);
    MICROPY_PY_LWIP_EXIT
    return len;
}

void tud_network_init_cb(void) {
    // Called by TinyUSB's ECM/RNDIS driver when the host configures the
    // interface. The NCM driver (TinyUSB >= 0.19) does NOT call this;
    // instead ncm_auto_init() is called from mod_network_init() before
    // USB starts. This implementation is kept for ECM/RNDIS compatibility.
    if (!ncm_obj.init) {
        ncm_init();
    }
    MICROPY_PY_LWIP_ENTER
    netif_set_link_up(&ncm_obj.netif);
    MICROPY_PY_LWIP_EXIT
}

// Called from mod_network_init() to set up the NCM netif before USB starts.
// TinyUSB's NCM driver begins receiving as soon as the host sends
// SET_INTERFACE(alt=1), which happens during USB enumeration -- before any
// Python code runs.  The netif must exist by then so tud_network_recv_cb()
// can deliver packets to lwIP.
//
// The netif is intentionally kept alive across MicroPython soft reset: lwIP
// is not re-initialised on soft reset, so the netif remains valid and TinyUSB
// can continue delivering packets. On re-entry we only re-register the singleton
// with the NIC list (which mod_network_init() cleared) rather than re-running
// netif_add (which must only be called once per netif).
void ncm_auto_init(void) {
    if (!ncm_obj.init) {
        ncm_init();
    } else {
        // On soft-reset mod_network_init() clears the NIC list, so
        // re-register the (still-live) singleton.
        mod_network_register_nic(&ncm_obj);
    }
    netif_set_up(&ncm_obj.netif);
    netif_set_link_up(&ncm_obj.netif);
}

/*******************************************************************************/
// MicroPython bindings

// Create and return a USBD_NCM object.
static mp_obj_t ncm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    if (!ncm_obj.init) {
        ncm_init();
    }
    // Return NCM object
    return MP_OBJ_FROM_PTR(&ncm_obj);
}

static mp_obj_t ncm_status(mp_obj_t self_in) {
    ncm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(IS_ACTIVE(self) ? 1 : 0);
}
static MP_DEFINE_CONST_FUN_OBJ_1(ncm_status_obj, ncm_status);

static mp_obj_t ncm_isconnected(mp_obj_t self_in) {
    ncm_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(tud_connected() && netif_is_link_up(&self->netif));
}
static MP_DEFINE_CONST_FUN_OBJ_1(ncm_isconnected_obj, ncm_isconnected);

static mp_obj_t ncm_active(size_t n_args, const mp_obj_t *args) {
    ncm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(IS_ACTIVE(self));
    } else {
        if (mp_obj_is_true(args[1])) {
            if (!IS_ACTIVE(self)) {
                netif_set_up(&self->netif);
                netif_set_link_up(&self->netif);
            }
        } else {
            if (IS_ACTIVE(self)) {
                netif_set_link_down(&self->netif);
                netif_set_down(&self->netif);
                // Note: We don't call ncm_deinit() here because that would
                // completely remove the network interface. We just want to
                // deactivate it so it can be reactivated later.
            }
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ncm_active_obj, 1, 2, ncm_active);

static mp_obj_t ncm_ifconfig(size_t n_args, const mp_obj_t *args) {
    ncm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->netif, n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ncm_ifconfig_obj, 1, 2, ncm_ifconfig);

static mp_obj_t ncm_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    ncm_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->netif, n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(ncm_ipconfig_obj, 1, ncm_ipconfig);

static mp_obj_t ncm_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args == 2 && kwargs->used == 0) {
        const char *key = mp_obj_str_get_str(args[1]);
        if (strcmp(key, "mac") == 0) {
            return mp_obj_new_bytes(tud_network_mac_address, sizeof(tud_network_mac_address));
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
}
static MP_DEFINE_CONST_FUN_OBJ_KW(ncm_config_obj, 1, ncm_config);

static const mp_rom_map_elem_t ncm_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&ncm_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&ncm_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&ncm_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&ncm_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&ncm_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&ncm_config_obj) },
};
static MP_DEFINE_CONST_DICT(ncm_locals_dict, ncm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_ncm,
    MP_QSTR_USBD_NCM,
    MP_TYPE_FLAG_NONE,
    make_new, ncm_make_new,
    locals_dict, &ncm_locals_dict
    );

#endif
