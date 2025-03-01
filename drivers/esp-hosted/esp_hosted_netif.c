/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2023 Arduino SA
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
 * ESP-Hosted LWIP netif functions.
 */

#include "py/mphal.h"
#include "py/mperrno.h"

#if MICROPY_PY_NETWORK_ESP_HOSTED

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/etharp.h"

#include "shared/netutils/netutils.h"
#include "shared/netutils/dhcpserver.h"

#include "esp_hosted_hal.h"
#include "esp_hosted_wifi.h"
#include "esp_hosted_netif.h"
#include "esp_hosted_internal.h"

static err_t netif_struct_init(struct netif *netif) {
    netif->linkoutput = esp_hosted_netif_output;
    netif->output = etharp_output;
    netif->mtu = ESP_FRAME_MAX_PAYLOAD;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    esp_hosted_wifi_get_mac(netif->name[1] - '0', netif->hwaddr);
    netif->hwaddr_len = sizeof(netif->hwaddr);
    info_printf("netif_init() netif initialized\n");
    return ERR_OK;
}

int esp_hosted_netif_init(esp_hosted_state_t *state, uint32_t itf) {
    struct netif *netif = &state->netif[itf];

    ip_addr_t ipconfig[4];
    ipconfig[0].addr = 0;
    ipconfig[1].addr = 0;
    ipconfig[2].addr = 0;
    ipconfig[3].addr = 0;

    if (itf == ESP_HOSTED_AP_IF) {
        ipconfig[0].addr = PP_HTONL(ESP_HOSTED_AP_ADDRESS);
        ipconfig[1].addr = PP_HTONL(ESP_HOSTED_AP_NETMASK);
        ipconfig[2].addr = PP_HTONL(ESP_HOSTED_AP_GATEWAY);
    }

    netif->name[0] = 'w';
    netif->name[1] = '0' + itf;

    netif_add(netif, ip_2_ip4(&ipconfig[0]), ip_2_ip4(&ipconfig[1]),
        ip_2_ip4(&ipconfig[2]), state, netif_struct_init, ethernet_input);

    netif_set_hostname(netif, ESP_HOSTED_HOSTNAME);
    netif_set_default(netif);
    netif_set_up(netif);
    netif_set_link_up(netif);
    dns_setserver(0, &ipconfig[3]);

    if (itf == ESP_HOSTED_STA_IF) {
        dhcp_set_struct(netif, &state->dhcp_client);
        dhcp_start(netif);
    } else {
        dhcp_server_init(&state->dhcp_server, &ipconfig[0], &ipconfig[1]);
    }
    return 0;
}

int esp_hosted_netif_deinit(esp_hosted_state_t *state, uint32_t itf) {
    struct netif *netif = &state->netif[itf];

    if (netif_is_link_up(netif)) {
        if (itf == ESP_HOSTED_STA_IF) {
            dhcp_stop(netif);
        } else {
            dhcp_server_deinit(&state->dhcp_server);
        }
    }

    for (struct netif *netifp = netif_list; netif != NULL; netif = netif->next) {
        if (netif == netifp) {
            netif_remove(netif);
            netif->flags = 0;
            return 0;
        }
    }

    return -1;
}

int esp_hosted_netif_input(esp_hosted_state_t *state, uint32_t itf, const void *buf, size_t len) {
    struct netif *netif = &state->netif[itf];

    struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    if (p == NULL) {
        error_printf("esp_hosted_netif_input() failed to alloc pbuf %d\n", len);
        return -1;
    }
    // Copy buf to pbuf
    pbuf_take(p, buf, len);

    if (netif->input(p, netif) != ERR_OK) {
        error_printf("esp_hosted_netif_input() netif input failed\n");
        pbuf_free(p);
        return -1;
    }

    debug_printf("esp_hosted_netif_input() eth frame input %d\n", len);
    return 0;
}

err_t esp_hosted_netif_output(struct netif *netif, struct pbuf *p) {
    esp_hosted_state_t *state = netif->state;

    if (p->tot_len > ESP_FRAME_MAX_PAYLOAD) {
        error_printf("esp_hosted_netif_output() pbuf len > SPI buf len\n");
        return ERR_IF;
    }

    esp_header_t *esp_header = (esp_header_t *)(state->buf);
    esp_header->if_type = netif->name[1] - '0';
    esp_header->if_num = 0;
    esp_header->flags = 0;
    esp_header->len = p->tot_len;
    esp_header->offset = sizeof(esp_header_t);
    esp_header->seq_num = 0;
    pbuf_copy_partial(p, esp_header->payload, p->tot_len, 0);
    esp_header->checksum = esp_hosted_checksum(esp_header);

    size_t frame_size = (sizeof(esp_header_t) + esp_header->len + 3) & ~3U;
    if (esp_hosted_hal_spi_transfer(state->buf, NULL, frame_size) != 0) {
        error_printf("esp_hosted_netif_output() failed to send eth frame\n");
        return ERR_IF;
    }
    debug_printf("esp_hosted_netif_output() if %d pbuf len %d\n", esp_header->if_type, esp_header->len);
    return ERR_OK;
}
#endif
