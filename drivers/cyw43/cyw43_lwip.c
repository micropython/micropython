/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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
#include <string.h>

#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/apps/mdns.h"
#include "drivers/cyw43/cyw43.h"

STATIC void cyw43_ethernet_trace(cyw43_t *self, struct netif *netif, size_t len, const void *data, unsigned int flags) {
    bool is_tx = flags & NETUTILS_TRACE_IS_TX;
    if ((is_tx && (self->trace_flags & CYW43_TRACE_ETH_TX))
        || (!is_tx && (self->trace_flags & CYW43_TRACE_ETH_RX))) {
        const uint8_t *buf;
        if (len == (size_t)-1) {
            // data is a pbuf
            const struct pbuf *pbuf = data;
            buf = pbuf->payload;
            len = pbuf->len; // restricted to print only the first chunk of the pbuf
        } else {
            // data is actual data buffer
            buf = data;
        }

        if (self->trace_flags & CYW43_TRACE_MAC) {
            printf("[% 8d] ETH%cX itf=%c%c len=%u", (int)mp_hal_ticks_ms(), is_tx ? 'T' : 'R', netif->name[0], netif->name[1], len);
            printf(" MAC type=%d subtype=%d data=", buf[0] >> 2 & 3, buf[0] >> 4);
            for (size_t i = 0; i < len; ++i) {
                printf(" %02x", buf[i]);
            }
            printf("\n");
            return;
        }

        if (self->trace_flags & CYW43_TRACE_ETH_FULL) {
            flags |= NETUTILS_TRACE_PAYLOAD;
        }
        netutils_ethernet_trace(MP_PYTHON_PRINTER, len, buf, flags);
    }
}

STATIC err_t cyw43_netif_output(struct netif *netif, struct pbuf *p) {
    cyw43_t *self = netif->state;
    if (self->trace_flags != 0) {
        cyw43_ethernet_trace(self, netif, (size_t)-1, p, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);
    }
    int itf = netif->name[1] - '0';
    int ret = cyw43_send_ethernet(self, itf, p->tot_len, (void*)p, true);
    if (ret) {
        printf("[CYW43] send_ethernet failed: %d\n", ret);
        return ERR_IF;
    }
    return ERR_OK;
}

STATIC err_t cyw43_netif_init(struct netif *netif) {
    netif->linkoutput = cyw43_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    cyw43_wifi_get_mac(netif->state, netif->name[1] - '0', netif->hwaddr);
    netif->hwaddr_len = sizeof(netif->hwaddr);
    return ERR_OK;
}

void cyw43_tcpip_init(cyw43_t *self, int itf) {
    ip_addr_t ipconfig[4];
    #if LWIP_IPV6
    #define IP(x) ((x).u_addr.ip4)
    #else
    #define IP(x) (x)
    #endif
    if (itf == 0) {
        // need to zero out to get isconnected() working
        IP4_ADDR(&IP(ipconfig[0]), 0, 0, 0, 0);
        IP4_ADDR(&IP(ipconfig[2]), 192, 168, 0, 1);
    } else {
        IP4_ADDR(&IP(ipconfig[0]), 192, 168, 4, 1);
        IP4_ADDR(&IP(ipconfig[2]), 192, 168, 4, 1);
    }
    IP4_ADDR(&IP(ipconfig[1]), 255, 255, 255, 0);
    IP4_ADDR(&IP(ipconfig[3]), 8, 8, 8, 8);
    #undef IP

    struct netif *n = &self->netif[itf];
    n->name[0] = 'w';
    n->name[1] = '0' + itf;
    #if LWIP_IPV6
    netif_add(n, &ipconfig[0].u_addr.ip4, &ipconfig[1].u_addr.ip4, &ipconfig[2].u_addr.ip4, self, cyw43_netif_init, ethernet_input);
    #else
    netif_add(n, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, cyw43_netif_init, netif_input);
    #endif
    netif_set_hostname(n, "PYBD");
    netif_set_default(n);
    netif_set_up(n);

    if (itf == CYW43_ITF_STA) {
        dns_setserver(0, &ipconfig[3]);
        dhcp_set_struct(n, &self->dhcp_client);
        dhcp_start(n);
    } else {
        dhcp_server_init(&self->dhcp_server, &ipconfig[0], &ipconfig[1]);
    }

    #if LWIP_MDNS_RESPONDER
    // TODO better to call after IP address is set
    char mdns_hostname[9];
    memcpy(&mdns_hostname[0], "PYBD", 4);
    mp_hal_get_mac_ascii(MP_HAL_MAC_WLAN0, 8, 4, &mdns_hostname[4]);
    mdns_hostname[8] = '\0';
    mdns_resp_add_netif(n, mdns_hostname, 60);
    #endif
}

void cyw43_tcpip_deinit(cyw43_t *self, int itf) {
    struct netif *n = &self->netif[itf];
    if (itf == CYW43_ITF_STA) {
        dhcp_stop(n);
    } else {
        dhcp_server_deinit(&self->dhcp_server);
    }
    #if LWIP_MDNS_RESPONDER
    mdns_resp_remove_netif(n);
    #endif
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == n) {
            netif_remove(netif);
            netif->ip_addr.addr = 0;
            netif->flags = 0;
        }
    }
}

void cyw43_cb_process_ethernet(void *cb_data, int itf, size_t len, const uint8_t *buf) {
    cyw43_t *self = cb_data;
    struct netif *netif = &self->netif[itf];
    if (self->trace_flags) {
        cyw43_ethernet_trace(self, netif, len, buf, NETUTILS_TRACE_NEWLINE);
    }
    if (netif->flags & NETIF_FLAG_LINK_UP) {
        struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
        if (p != NULL) {
            pbuf_take(p, buf, len);
            if (netif->input(p, netif) != ERR_OK) {
                pbuf_free(p);
            }
        }
    }
}

void cyw43_tcpip_set_link_up(cyw43_t *self, int itf) {
    netif_set_link_up(&self->netif[itf]);
}

void cyw43_tcpip_set_link_down(cyw43_t *self, int itf) {
    netif_set_link_down(&self->netif[itf]);
}

int cyw43_tcpip_link_status(cyw43_t *self, int itf) {
    struct netif *netif = &self->netif[itf];
    if ((netif->flags & (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP))
        == (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) {
        if (netif->ip_addr.addr != 0) {
            return CYW43_LINK_UP;
        } else {
            return CYW43_LINK_NOIP;
        }
    } else {
        return cyw43_wifi_link_status(self, itf);
    }
}
