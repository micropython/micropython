#include <stdio.h>
#include <string.h>
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"

#include "lwip/opt.h"
#include "lwip_inc/lwipopts.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/dns.h"
#include "lwip/igmp.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"
#include "modnetwork.h"
#include "ethernetif.h"

#if MICROPY_HW_ENABLE_ETH_RMII

/*******************************************************************************/
// STM32 Ethernet driver

typedef struct _ethernet_obj_t {
    mod_network_nic_type_t base;
    struct netif netif;
    struct dhcp dhcp_struct;
    mp_uint_t last_arp;
} ethernet_obj_t;

STATIC void ethernet_lwip_init(ethernet_obj_t *self);
STATIC void ethernet_lwip_poll(void *self_in, struct netif *netif);

const mp_obj_type_t mod_network_nic_type_ethernet;

STATIC ethernet_obj_t ethernet_obj;

// network.Ethernet()

STATIC mp_obj_t ethernet_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // init the ethernet object
    ethernet_obj.base.base.base.type = &mod_network_nic_type_ethernet;
    ethernet_obj.base.poll_callback = ethernet_lwip_poll;

    // Hardware init
    ethernet_lwip_init(&ethernet_obj);

    // return ethernet object
    return &ethernet_obj;
}

STATIC mp_obj_t ethernet_isconnected(mp_obj_t self_in) {
    ethernet_obj_t *self = self_in;
    ethernetif_set_link(&self->netif);
    return mp_obj_new_bool(self->netif.flags & NETIF_FLAG_LINK_UP);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(ethernet_isconnected_obj, ethernet_isconnected);

STATIC mp_obj_t ethernet_active(size_t n_args, const mp_obj_t *args) {
    ethernet_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(self->netif.flags & NETIF_FLAG_UP);
    } else {
        if (mp_obj_is_true(args[1])) {
            netif_set_up(&self->netif);
            netif_set_link_up(&self->netif);
        } else {
            netif_set_link_down(&self->netif);
            netif_set_down(&self->netif);
        }
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ethernet_active_obj, 1, 2, ethernet_active);

STATIC mp_obj_t ethernet_ifconfig(size_t n_args, const mp_obj_t *args) {
    ethernet_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args == 1) {
        // get
        const ip_addr_t *dns = dns_getserver(0);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t*)&self->netif.ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&self->netif.netmask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&self->netif.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t*)&dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else if (args[1] == MP_OBJ_NEW_QSTR(MP_QSTR_dhcp)) {
        dhcp_set_struct(&self->netif, &self->dhcp_struct);
        dhcp_start(&self->netif);

        // wait for dhcp to get IP address
        uint32_t start = mp_hal_ticks_ms();
        while (!dhcp_supplied_address(&self->netif) && mp_hal_ticks_ms() - start < 8000) {
            mp_hal_delay_ms(100);
        }
        if (!dhcp_supplied_address(&self->netif)) {
            mp_raise_msg(&mp_type_OSError, "timeout waiting for DHCP to get IP address");
        }
        return mp_const_none;
    } else {
        // set
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], (uint8_t*)&self->netif.ip_addr, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], (uint8_t*)&self->netif.netmask, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], (uint8_t*)&self->netif.gw, NETUTILS_BIG);
        ip_addr_t dns;
        netutils_parse_ipv4_addr(items[3], (uint8_t*)&dns, NETUTILS_BIG);
        dns_setserver(0, &dns);
	ethernetif_update_config(&self->netif);
        return mp_const_none;
    }
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ethernet_ifconfig_obj, 1, 2, ethernet_ifconfig);

/*******************************************************************************/
// lwip stuff

STATIC void ethernet_lwip_init(ethernet_obj_t *self) {
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &self->netif) {
            netif_remove(netif);
            break;
        }
    }
    ip_addr_t ipconfig[4];
    ipconfig[0].addr = 0;
    ipconfig[1].addr = 0;
    ipconfig[2].addr = 0;
    ipconfig[3].addr = 0;

    memset(&self->netif, 0, sizeof(struct netif));
    self-> netif.linkoutput = low_level_output;

    netif_add(&self->netif, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, ethernetif_init, ethernet_input);
    netif_set_default(&self->netif);
    dns_setserver(0, &ipconfig[3]);
}

STATIC void ethernet_lwip_poll(void *self_in, struct netif *netif) {
    ethernet_obj_t *self = self_in;
    mp_uint_t t;

    ethernetif_input(&self->netif);
    t = mp_hal_ticks_ms();
    if ((t - self->last_arp) > ARP_TMR_INTERVAL) {
      etharp_tmr();
      self->last_arp = t;
    }
}

/*******************************************************************************/
// class bindings

STATIC const mp_rom_map_elem_t ethernet_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&ethernet_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&ethernet_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&ethernet_ifconfig_obj) },

};
STATIC MP_DEFINE_CONST_DICT(ethernet_locals_dict, ethernet_locals_dict_table);

const mp_obj_type_t mod_network_nic_type_ethernet = {
    { &mp_type_type },
    .name = MP_QSTR_Ethernet,
    .make_new = ethernet_make_new,
    .locals_dict = (mp_obj_dict_t*)&ethernet_locals_dict,
};

#endif // MICROPY_HW_ENABLE_ETH_RMII
