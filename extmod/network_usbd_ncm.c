/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Andrew Leech
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

#if MICROPY_HW_NETWORK_USBNET

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

#if LWIP_IPV6
#define IP(x) ((x).u_addr.ip4)
#else
#define IP(x) (x)
#endif

typedef struct _usbnet_obj_t {
    mp_obj_base_t base;

    struct netif netif;
    bool active;

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gateway;

    struct pbuf *received_frame; // shared between tud_network_recv_cb() and service_traffic()

    #if MICROPY_HW_NETWORK_USBNET_DHCP_SERVER
    dhcp_server_t dhcp_server;
    #endif
} usbnet_obj_t;

// Global object holding the usb net state
static usbnet_obj_t usbnet_obj;

#define IS_ACTIVE(self) (self->netif.flags & NETIF_FLAG_UP)

/* This default mac will be updated during init from hardware unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
uint8_t tud_network_mac_address[6] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};


static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
    (void)netif;

    for (int i = 100; i; i--) {  // limit retries; original tinyusb example has no limit.
        // if TinyUSB isn't ready, we must signal back to lwip that there is nothing we can do
        if (!tud_ready()) {
            return ERR_USE;
        }

        // if the network driver can accept another packet, we make it happen
        if (tud_network_can_xmit(p->tot_len)) {
            tud_network_xmit(p, 0 /* unused for this example */);
            return ERR_OK;
        }

        // transfer execution to TinyUSB in the hopes that it will finish transmitting the prior packet
        tud_task();

        mp_event_handle_nowait();
    }
    return ERR_USE;
}

static err_t ip4_output_fn(struct netif *netif, struct pbuf *p, const ip4_addr_t *addr) {
    return etharp_output(netif, p, addr);
}

#if LWIP_IPV6
static err_t ip6_output_fn(struct netif *netif, struct pbuf *p, const ip6_addr_t *addr) {
    return ethip6_output(netif, p, addr);
}
#endif

static err_t netif_init_cb(struct netif *netif) {
    LWIP_ASSERT("netif != NULL", (netif != NULL));
    netif->mtu = CFG_TUD_NET_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = ip4_output_fn;
    #if LWIP_IPV6
    netif->output_ip6 = ip6_output_fn;
    netif->flags |= NETIF_FLAG_MLD6;
    #endif
    #if LWIP_IGMP
    // netif_set_igmp_mac_filter(netif, cyw43_netif_update_igmp_mac_filter);
    #endif
    return ERR_OK;
}

#if MICROPY_HW_NETWORK_USBNET_DHCP_SERVER
void dhcp_client_connected(uint8_t ciaddr[4], uint8_t chaddr[16]) {
    // When a host pc connects and is assigned an ip address, use that host
    // as the default gateway. This means the host could optionally be configured
    // to forward internet to the usb network adaptor and this device will be able to use it.
    ip4_addr_t gw4;
    IP4_ADDR(&gw4, ciaddr[0], ciaddr[1], ciaddr[2], ciaddr[3]);
    netif_set_gw(&(usbnet_obj.netif), &gw4);

    #if LWIP_MDNS_RESPONDER
    mdns_resp_netif_settings_changed(&(usbnet_obj.netif));
    #endif
}
#endif

void usbnet_init(void) {
    struct netif *netif = &(usbnet_obj.netif);

    MICROPY_PY_LWIP_ENTER

    mp_hal_get_mac(MP_HAL_MAC_ETH0, tud_network_mac_address);
    tud_network_mac_address[0] = 0x02;
    // tud_network_mac_address[1] = 0x00;
    // tud_network_mac_address[2] = 0x5E;
    // tud_network_mac_address[5] ^= 0x1;

    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));

    // This USB Network essentially creates a link with two interfaces, one for the
    // host and another for the micropython/lwip end of the connection.
    // The MAC configured above is use for one end of the link, flip a bit for the
    // MAC on the other end of the link.
    netif->hwaddr[5] ^= 0x01;

    IP(usbnet_obj.ipaddr).addr = PP_HTONL(MICROPY_HW_NETWORK_USBNET_IPADDRESS);
    IP(usbnet_obj.netmask).addr = PP_HTONL(MICROPY_HW_NETWORK_USBNET_SUBNET);
    IP(usbnet_obj.gateway).addr = PP_HTONL(MICROPY_HW_NETWORK_USBNET_GATEWAY);

    netif = netif_add(
        netif,
        ip_2_ip4(&usbnet_obj.ipaddr),
        ip_2_ip4(&usbnet_obj.netmask),
        ip_2_ip4(&usbnet_obj.gateway),
        NULL,
        netif_init_cb,
        ip_input
        );
    #if LWIP_IPV6
    netif_create_ip6_linklocal_address(netif, 1);
    netif_set_ip6_autoconfig_enabled(netif, 1);
    #endif
    #if LWIP_NETIF_HOSTNAME
    netif_set_hostname(netif, mod_network_hostname_data);
    #endif
    netif_set_default(netif);
    netif_set_up(netif);
    netif_set_link_up(netif);

    #if LWIP_MDNS_RESPONDER
    mdns_resp_add_netif(netif, mod_network_hostname_data);
    #endif

    #if MICROPY_HW_NETWORK_USBNET_DHCP_SERVER
    dhcp_server_init(&usbnet_obj.dhcp_server, &usbnet_obj.ipaddr, &usbnet_obj.netmask);
    dhcp_server_register_connect_cb(&usbnet_obj.dhcp_server, &dhcp_client_connected);
    #endif

    // register with network module
    mod_network_register_nic(&usbnet_obj);

    MICROPY_PY_LWIP_EXIT
}

void usbnet_deinit(void) {
    struct netif *netif = &(usbnet_obj.netif);

    #if LWIP_MDNS_RESPONDER
    mdns_resp_remove_netif(netif);
    #endif

    #if MICROPY_HW_NETWORK_USBNET_DHCP_SERVER
    dhcp_server_deinit(&usbnet_obj.dhcp_server);
    #endif

    // remove from lwip
    netif_remove(netif);
}

bool tud_network_recv_cb(const uint8_t *src, uint16_t size) {
    // this shouldn't happen, but if we get another packet before
    // parsing the previous, we must signal our inability to accept it
    if (usbnet_obj.received_frame) {
        return false;
    }

    if (size) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);

        if (p) {
            // pbuf_alloc() has already initialized struct; all we need to do is copy the data
            memcpy(p->payload, src, size);

            // store away the pointer for service_traffic() to later handle
            usbnet_obj.received_frame = p;
        }
    }

    return true;
}

uint16_t tud_network_xmit_cb(uint8_t *dst, void *ref, uint16_t arg) {
    struct pbuf *p = (struct pbuf *)ref;

    (void)arg;

    return pbuf_copy_partial(p, dst, p->tot_len, 0);
}

void network_usbd_ncm_service_traffic(void) {
    // handle any packet received by tud_network_recv_cb()
    if (usbnet_obj.received_frame) {
        ethernet_input(usbnet_obj.received_frame, &(usbnet_obj.netif));
        pbuf_free(usbnet_obj.received_frame);
        usbnet_obj.received_frame = NULL;
        tud_network_recv_renew();
    }

    sys_check_timeouts();
}

void tud_network_init_cb(void) {
    // if the network is re-initializing and we have a leftover packet, we must do a cleanup
    if (usbnet_obj.received_frame) {
        pbuf_free(usbnet_obj.received_frame);
        usbnet_obj.received_frame = NULL;
    }
}

/*******************************************************************************/
// MicroPython bindings

// Create and return a USB_NET object.
static mp_obj_t usbnet_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {

    // Init the usbnet object
    usbnet_obj.base.type = (mp_obj_type_t *)&mod_network_nic_type_usbnet;
    usbnet_obj.active = false;

    // Return usbnet object
    return MP_OBJ_FROM_PTR(&usbnet_obj);
}

static mp_obj_t usbnet_isconnected(mp_obj_t self_in) {
    usbnet_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(IS_ACTIVE(self));
}
static MP_DEFINE_CONST_FUN_OBJ_1(usbnet_isconnected_obj, usbnet_isconnected);

static mp_obj_t usbnet_active(size_t n_args, const mp_obj_t *args) {
    usbnet_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(IS_ACTIVE(self));
    } else {
        if (mp_obj_is_true(args[1])) {
            if (!IS_ACTIVE(self)) {
                usbnet_init();
            }
        } else {
            if (IS_ACTIVE(self)) {
                usbnet_deinit();
            }
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usbnet_active_obj, 1, 2, usbnet_active);


static mp_obj_t usbnet_ifconfig(size_t n_args, const mp_obj_t *args) {
    usbnet_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->netif, n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(usbnet_ifconfig_obj, 1, 2, usbnet_ifconfig);

static mp_obj_t network_usbnet_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    usbnet_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->netif, n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(usbnet_ipconfig_obj, 1, network_usbnet_ipconfig);

static const mp_rom_map_elem_t usbnet_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&usbnet_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usbnet_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&usbnet_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&usbnet_ipconfig_obj) },
};
static MP_DEFINE_CONST_DICT(usbnet_locals_dict, usbnet_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_usbnet,
    MP_QSTR_USB_NET,
    MP_TYPE_FLAG_NONE,
    make_new, usbnet_make_new,
    locals_dict, &usbnet_locals_dict
    );

#endif
