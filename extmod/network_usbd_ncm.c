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

#include "extmod/modnetwork.h"
#include "shared/netutils/netutils.h"
#include "shared/netutils/dhcpserver.h"


typedef struct _usbnet_obj_t {
    mp_obj_base_t base;

    struct netif netif;

    struct pbuf *received_frame; // shared between tud_network_recv_cb() and service_traffic()
    struct dhcp dhcp_struct;
    dhcp_server_t dhcp_server;
    bool active;

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gateway;

} usbnet_obj_t;

// Global object holding the usb net state
static usbnet_obj_t usbnet_obj;

#define IS_ACTIVE(self) (self->netif.flags & NETIF_FLAG_UP)

/* This default mac will be updated during init from hardware unique ID (if available) */
/* it is suggested that the first byte is 0x02 to indicate a link-local address */
uint8_t tud_network_mac_address[6] = {0x02, 0x02, 0x84, 0x6A, 0x96, 0x00};


static err_t linkoutput_fn(struct netif *netif, struct pbuf *p) {
    (void)netif;

    for (int i = 3; i; i--) {  // limit retries; original tinyusb example has no limit.
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

        #if MICROPY_PY_THREAD
        MICROPY_THREAD_YIELD();
        #else
        mp_handle_pending(true);
        #endif
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
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP | NETIF_FLAG_UP;
    netif->state = NULL;
    netif->name[0] = 'E';
    netif->name[1] = 'X';
    netif->linkoutput = linkoutput_fn;
    netif->output = ip4_output_fn;
    #if LWIP_IPV6
    netif->output_ip6 = ip6_output_fn;
    #endif
    return ERR_OK;
}

void usbnet_init(void) {
    struct netif *netif = &(usbnet_obj.netif);

    MICROPY_PY_LWIP_ENTER

    mp_hal_get_mac(MP_HAL_MAC_ETH0, tud_network_mac_address);

    netif->hwaddr_len = sizeof(tud_network_mac_address);
    memcpy(netif->hwaddr, tud_network_mac_address, sizeof(tud_network_mac_address));
    netif->hwaddr[5] ^= 0x01;

    #if LWIP_IPV6
    #define IP(x) ((x).u_addr.ip4)
    #else
    #define IP(x) (x)
    #endif

    IP(usbnet_obj.ipaddr).addr = PP_HTONL(LWIP_MAKEU32(192, 168, 7, 1));
    IP(usbnet_obj.netmask).addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0));
    IP(usbnet_obj.gateway).addr = PP_HTONL(LWIP_MAKEU32(0, 0, 0, 0));

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
    #endif
    netif_set_default(netif);

    dhcp_server_init(&usbnet_obj.dhcp_server, &usbnet_obj.ipaddr, &usbnet_obj.netmask);

    MICROPY_PY_LWIP_EXIT
}

void usbnet_deinit(void) {
    // TODO
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

// Create and return a USBNET object.
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

// TODO
// static mp_obj_t usbnet_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
//     usbnet_obj_t *self = MP_OBJ_TO_PTR(args[0]);

//     if (kwargs->used == 0) {
//         // Get config value
//         if (n_args != 2) {
//             mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
//         }

//         switch (mp_obj_str_get_qstr(args[1])) {
//             case MP_QSTR_mac: {
//                 uint8_t buf[6];
//                 usbnet_get_mac_address(self, buf);
//                 return mp_obj_new_bytes(buf, 6);
//             }
//             default:
//                 mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
//         }
//     } else {
//         // Set config value(s)
//         if (n_args != 1) {
//             mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
//         }

//         for (size_t i = 0; i < kwargs->alloc; ++i) {
//             if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
//                 mp_map_elem_t *e = &kwargs->table[i];
//                 switch (mp_obj_str_get_qstr(e->key)) {
//                     case MP_QSTR_mac: {
//                         mp_buffer_info_t buf;
//                         mp_get_buffer_raise(e->value, &buf, MP_BUFFER_READ);
//                         if (buf.len != 6) {
//                             mp_raise_ValueError(NULL);
//                         }
//                         setSHAR(buf.buf);
//                         #if USBNET_WITH_LWIP_STACK
//                         memcpy(self->netif.hwaddr, buf.buf, 6);
//                         #endif
//                         break;
//                     }
//                     #if USBNET_WITH_LWIP_STACK
//                     case MP_QSTR_trace: {
//                         self->trace_flags = mp_obj_get_int(e->value);
//                         break;
//                     }
//                     #endif
//                     default:
//                         mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
//                 }
//             }
//         }

//         return mp_const_none;
//     }
// }
// static MP_DEFINE_CONST_FUN_OBJ_KW(usbnet_config_obj, 1, usbnet_config);

static const mp_rom_map_elem_t usbnet_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&usbnet_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&usbnet_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&usbnet_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&usbnet_ipconfig_obj) },
    // { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&usbnet_config_obj) },
};
static MP_DEFINE_CONST_DICT(usbnet_locals_dict, usbnet_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_usbnet,
    MP_QSTR_USBNET,
    MP_TYPE_FLAG_NONE,
    make_new, usbnet_make_new,
    locals_dict, &usbnet_locals_dict
    );

#endif
