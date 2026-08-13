/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 OpenMV LLC.
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
 *
 * lwIP interface for the Morse Micro 802.11ah driver.
 */

#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_HALOW

#include <string.h>

#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/ethip6.h"
#include "lwip/igmp.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "netif/ethernet.h"

#include "extmod/modnetwork.h"

#include "halow.h"

static err_t halow_netif_output(struct netif *netif, struct pbuf *p) {
    halow_t *self = netif->state;
    int itf = netif->name[1] - '0';

    if (self->trace_flags & HALOW_TRACE_ETH_TX) {
        mp_printf(&mp_plat_print, "halow: [txf] itf=%d len=%u\n", itf, (unsigned int)p->tot_len);
    }

    int ret = halow_send_ethernet(self, itf, p->tot_len, p, true);
    if (ret == -MP_EAGAIN) {
        // The transmit queue is still full after the driver has been serviced
        // for HALOW_TX_READY_MS.  Drop the frame, which is what an interface
        // does when its queue is full: TCP retransmits, UDP is lossy by
        // definition.  Reporting ERR_MEM instead surfaces a transient queue
        // full to the application as ENOMEM, which it cannot act on.
        return ERR_OK;
    }
    if (ret != 0) {
        return ERR_IF;
    }
    return ERR_OK;
}

#if LWIP_IGMP
static err_t halow_netif_update_igmp_mac_filter(struct netif *netif, const ip4_addr_t *group,
    enum netif_mac_filter_action action) {
    // The transceiver does not filter multicast in hardware; lwIP does it.
    (void)netif;
    (void)group;
    (void)action;
    return ERR_OK;
}
#endif

static err_t halow_netif_init(struct netif *netif) {
    halow_t *self = netif->state;
    int itf = netif->name[1] - '0';

    netif->linkoutput = halow_netif_output;
    netif->output = etharp_output;
    #if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
    #endif
    netif->mtu = MICROPY_HW_HALOW_MTU;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET;

    halow_wifi_get_mac(self, itf, netif->hwaddr);
    netif->hwaddr_len = sizeof(netif->hwaddr);

    #if LWIP_IGMP
    netif->flags |= NETIF_FLAG_IGMP;
    netif_set_igmp_mac_filter(netif, halow_netif_update_igmp_mac_filter);
    #endif

    return ERR_OK;
}

void halow_cb_tcpip_init(halow_t *self, int itf) {
    struct netif *netif = &self->netif[itf];

    #if LWIP_IPV4
    ip_addr_t ipconfig[3];
    ip4_addr_set_zero(ip_2_ip4(&ipconfig[0]));
    ip4_addr_set_zero(ip_2_ip4(&ipconfig[1]));
    ip4_addr_set_zero(ip_2_ip4(&ipconfig[2]));
    if (itf == HALOW_ITF_AP) {
        ip_2_ip4(&ipconfig[0])->addr = PP_HTONL(MICROPY_HW_HALOW_AP_ADDRESS);
        ip_2_ip4(&ipconfig[1])->addr = PP_HTONL(MICROPY_HW_HALOW_AP_NETMASK);
        ip_2_ip4(&ipconfig[2])->addr = PP_HTONL(MICROPY_HW_HALOW_AP_ADDRESS);
    } else {
        #if !LWIP_DHCP
        // No client to ask, so come up on the configured address rather than on
        // 0.0.0.0, where the interface would never be usable.
        ip_2_ip4(&ipconfig[0])->addr = PP_HTONL(MICROPY_HW_HALOW_STA_ADDRESS);
        ip_2_ip4(&ipconfig[1])->addr = PP_HTONL(MICROPY_HW_HALOW_STA_NETMASK);
        ip_2_ip4(&ipconfig[2])->addr = PP_HTONL(MICROPY_HW_HALOW_STA_GATEWAY);
        #endif
    }
    #endif

    netif->name[0] = 'w';
    netif->name[1] = '0' + itf;

    #if LWIP_IPV4
    netif_add(netif, ip_2_ip4(&ipconfig[0]), ip_2_ip4(&ipconfig[1]), ip_2_ip4(&ipconfig[2]),
        self, halow_netif_init, ethernet_input);
    #elif LWIP_IPV6
    netif_add(netif, self, halow_netif_init, ethernet_input);
    #else
    #error "halow needs either IPv4 or IPv6"
    #endif
    #if LWIP_NETIF_HOSTNAME
    netif_set_hostname(netif, mod_network_hostname_data);
    #endif
    if (netif_default == NULL) {
        // Only claim the default route if nothing else holds it.  A board can
        // have an Ethernet or another wireless interface up and addressed, and
        // taking the default from it would send that traffic here instead.
        netif_set_default(netif);
    }
    netif_set_up(netif);

    if (itf == HALOW_ITF_STA) {
        #if LWIP_IPV4 && LWIP_DNS
        // Only when there is one to set: the station's comes from DHCP, and
        // writing the zero address here would clear whatever another interface
        // or the user had already put in the slot.
        if (!ip_addr_isany(&ipconfig[2])) {
            dns_setserver(0, &ipconfig[2]);
        }
        #endif
        #if LWIP_IPV4 && LWIP_DHCP
        dhcp_set_struct(netif, &self->dhcp_client);
        #endif
    } else {
        #if MICROPY_PY_NETWORK_HALOW_AP && LWIP_IPV4
        dhcp_server_init(&self->dhcp_server, &ipconfig[0], &ipconfig[1]);
        #endif
    }
}

void halow_cb_tcpip_deinit(halow_t *self, int itf) {
    struct netif *netif = &self->netif[itf];
    bool was_default = netif_default == netif;

    if (itf == HALOW_ITF_STA) {
        #if LWIP_IPV4 && LWIP_DHCP
        dhcp_stop(netif);
        #endif
    } else {
        #if MICROPY_PY_NETWORK_HALOW_AP
        dhcp_server_deinit(&self->dhcp_server);
        #endif
    }

    struct netif *n;
    NETIF_FOREACH(n) {
        if (n == netif) {
            netif_remove(netif);
            #if LWIP_IPV4
            ip4_addr_set_zero(ip_2_ip4(&netif->ip_addr));
            #endif
            netif->flags = 0;
            break;
        }
    }

    if (was_default && netif_default == NULL) {
        // netif_remove() drops the default when it removes the interface
        // holding it, and leaves the system with none: every off-link route
        // then fails even though another interface is up.  Hand it to whatever
        // is left.
        NETIF_FOREACH(n) {
            if (netif_is_up(n)) {
                netif_set_default(n);
                break;
            }
        }
    }
}

void halow_cb_tcpip_set_link_up(halow_t *self, int itf) {
    struct netif *netif = &self->netif[itf];
    if (netif_is_link_up(netif)) {
        return;
    }
    netif_set_link_up(netif);
    #if LWIP_IPV4 && LWIP_DHCP
    if (itf == HALOW_ITF_STA) {
        dhcp_start(netif);
    }
    #endif
}

void halow_cb_tcpip_set_link_down(halow_t *self, int itf) {
    struct netif *netif = &self->netif[itf];
    if (!netif_is_link_up(netif)) {
        return;
    }
    // Only the link goes down here, as in cyw43.  Stopping DHCP would send a
    // release, and this is reached from the fatal error handler, where the
    // transceiver is in no state to transmit anything.  The lease is dropped in
    // halow_cb_tcpip_deinit() instead.
    netif_set_link_down(netif);
}

void halow_cb_process_ethernet(void *cb_data, int itf,
    const uint8_t *header, size_t header_len, const uint8_t *payload, size_t payload_len) {
    halow_t *self = cb_data;
    struct netif *netif = &self->netif[itf];
    size_t len = header_len + payload_len;

    if (self->rx_deferred) {
        // The driver is being run from inside lwIP, by the wait in
        // halow_send_ethernet().  Handing it a frame here would re-enter it
        // while it is walking its own lists.
        return;
    }

    if (self->trace_flags & HALOW_TRACE_ETH_RX) {
        mp_printf(&mp_plat_print, "halow: [rxf] itf=%d len=%u\n", itf, (unsigned int)len);
    }

    if (!netif_is_link_up(netif)) {
        return;
    }

    struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p == NULL) {
        return;
    }
    // The header morselib reports lives on its stack, so it has to be copied in
    // separately from the payload rather than treated as one buffer.
    if (pbuf_take(p, header, header_len) != ERR_OK ||
        pbuf_take_at(p, payload, payload_len, header_len) != ERR_OK) {
        pbuf_free(p);
        return;
    }

    if (netif->input(p, netif) != ERR_OK) {
        pbuf_free(p);
    }
}

#endif // MICROPY_PY_NETWORK_HALOW
