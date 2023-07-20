/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 * Copyright (c) 2023 Vekatech Ltd.
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

#include <string.h>
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"
#include "extmod/modnetwork.h"
#include "hal_data.h"
#include "eth.h"

#if defined(MICROPY_HW_ETH_MDC)

#include "lwip/etharp.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "netif/ethernet.h"


#define ETHER_EXAMPLE_ETHER_ISR_EE_FR_MASK     (1UL << 18)
#define ETHER_EXAMPLE_ETHER_ISR_EE_TC_MASK     (1UL << 21)
#define ETHER_EXAMPLE_ETHER_ISR_EC_MPD_MASK    (1UL << 1)

typedef struct _eth_t {
    uint32_t trace_flags;
    struct netif netif;
    struct dhcp dhcp_struct;
} eth_t;

uint8_t tx_TMPbuf[1536] __attribute__((aligned(4))); /* g_ether0_cfg.ether_buffer_size */
uint8_t rx_TMPbuf[1536] __attribute__((aligned(4))); /* g_ether0_cfg.ether_buffer_size */

eth_t eth_instance;
uint8_t phy_link_status = 0;
const machine_pin_obj_t *phy_RST = pin_P400;

// ETH-LwIP bindings

#define TRACE_ASYNC_EV (0x0001)
#define TRACE_ETH_TX (0x0002)
#define TRACE_ETH_RX (0x0004)
#define TRACE_ETH_FULL (0x0008)

STATIC void eth_trace(eth_t *self, size_t len, const void *data, unsigned int flags) {
    if (((flags & NETUTILS_TRACE_IS_TX) && (self->trace_flags & TRACE_ETH_TX))
        || (!(flags & NETUTILS_TRACE_IS_TX) && (self->trace_flags & TRACE_ETH_RX))) {
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
        if (self->trace_flags & TRACE_ETH_FULL) {
            flags |= NETUTILS_TRACE_PAYLOAD;
        }
        netutils_ethernet_trace(MP_PYTHON_PRINTER, len, buf, flags);
    }
}

STATIC void eth_process_frame(eth_t *self, size_t len, const uint8_t *buf) {
    eth_trace(self, len, buf, NETUTILS_TRACE_NEWLINE);

    struct netif *netif = &self->netif;
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

STATIC err_t eth_netif_output(struct netif *netif, struct pbuf *p) {
    // This function should always be called from a context where PendSV-level IRQs are disabled

    LINK_STATS_INC(link.xmit);
    eth_trace(netif->state, (size_t)-1, p, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);

    pbuf_copy_partial(p, tx_TMPbuf, p->tot_len, 0);
    if (FSP_SUCCESS == R_ETHER_Write(&g_ether0_ctrl, tx_TMPbuf, p->tot_len)) {
        return ERR_OK;
    } else {
        return ERR_BUF;
    }
}

STATIC err_t eth_netif_init(struct netif *netif) {
    netif->linkoutput = eth_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    // Checksums only need to be checked on incoming frames, not computed on outgoing frames
    /*NETIF_SET_CHECKSUM_CTRL(netif,
        NETIF_CHECKSUM_CHECK_IP
        | NETIF_CHECKSUM_CHECK_UDP
        | NETIF_CHECKSUM_CHECK_TCP
        | NETIF_CHECKSUM_CHECK_ICMP
        | NETIF_CHECKSUM_CHECK_ICMP6);
    */
    return ERR_OK;
}

STATIC void eth_lwip_init(eth_t *self) {
    // err_t e;

    ip_addr_t ipconfig[4];
    // IP4_ADDR(&ipconfig[0], 0, 0, 0, 0);
    // IP4_ADDR(&ipconfig[0], 192, 168, 0, 100);
    // IP4_ADDR(&ipconfig[2], 192, 168, 0, 1);
    IP4_ADDR(&ipconfig[0], 192, 168, 2, 188);
    IP4_ADDR(&ipconfig[2], 192, 168, 2, 254);
    IP4_ADDR(&ipconfig[1], 255, 255, 255, 0);
    IP4_ADDR(&ipconfig[3], 8, 8, 8, 8);

    MICROPY_PY_LWIP_ENTER

    struct netif *n = &self->netif;
    n->name[0] = 'e';
    n->name[1] = '0';
    netif_add(n, &ipconfig[0], &ipconfig[1], &ipconfig[2], self, eth_netif_init, ethernet_input);
    netif_set_hostname(n, (const char *)mod_network_hostname);
    netif_set_default(n);
    netif_set_up(n);

    dns_setserver(0, &ipconfig[3]);
    dhcp_set_struct(n, &self->dhcp_struct);
    dhcp_start(n);

    netif_set_link_up(n);

    // Wait for DHCP to get IP address
    uint32_t start = mp_hal_ticks_ms();
    while (!dhcp_supplied_address(n)) {
        if (mp_hal_ticks_ms() - start > 20000) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("DHCP failed to get IP address in 10 sec."));
        }
        mp_hal_delay_ms(200);
    }

    MICROPY_PY_LWIP_EXIT
}

STATIC void eth_lwip_deinit(eth_t *self) {
    MICROPY_PY_LWIP_ENTER
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &self->netif) {
            netif_remove(netif);
            netif->ip_addr.addr = 0;
            netif->flags = 0;
        }
    }
    MICROPY_PY_LWIP_EXIT
}

void ETH_IRQHandler(ether_callback_args_t *p_args) {

    switch (p_args->event)
    {
        case ETHER_EVENT_WAKEON_LAN:
            break;
        case ETHER_EVENT_LINK_ON:
            phy_link_status = 1;
            break;
        case ETHER_EVENT_LINK_OFF:
            phy_link_status = 0;
            break;

        case ETHER_EVENT_INTERRUPT: {
            if (ETHER_EXAMPLE_ETHER_ISR_EE_FR_MASK == (p_args->status_eesr & ETHER_EXAMPLE_ETHER_ISR_EE_FR_MASK)) {
                uint32_t len = 0;

                if (FSP_SUCCESS == R_ETHER_Read(&g_ether0_ctrl, rx_TMPbuf, &len)) {
                    eth_process_frame(&eth_instance, len, rx_TMPbuf);
                }
            }

            if (ETHER_EXAMPLE_ETHER_ISR_EE_TC_MASK == (p_args->status_eesr & ETHER_EXAMPLE_ETHER_ISR_EE_TC_MASK)) {
            }

            if (ETHER_EXAMPLE_ETHER_ISR_EC_MPD_MASK == (p_args->status_ecsr & ETHER_EXAMPLE_ETHER_ISR_EC_MPD_MASK)) {
            }
        }
        break;

        default: {
        }
    }
}

// ------------------------------------------------------------------------------

void eth_init(eth_t *self, int mac_idx) {
    fsp_err_t err;

    mp_hal_pin_output(phy_RST);

    mp_hal_pin_low(phy_RST);
    mp_hal_delay_us(200);
    mp_hal_pin_high(phy_RST);
    mp_hal_delay_us(200);

    if ((err = R_ETHER_Open(&g_ether0_ctrl, &g_ether0_cfg)) == FSP_SUCCESS) {
        self->netif.hwaddr_len = 6;         // self->netif.
        // mp_hal_get_mac(mac_idx, &self->netif.hwaddr[0]);
        memcpy(self->netif.hwaddr, g_ether0_cfg.p_mac_address, self->netif.hwaddr_len);
    }
}

void eth_set_trace(eth_t *self, uint32_t value) {
    self->trace_flags = value;
}

struct netif *eth_netif(eth_t *self) {
    return &self->netif;
}

int eth_link_status(eth_t *self) {

    struct netif *netif = &self->netif;
    if ((netif->flags & (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP))
        == (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) {
        if (netif->ip_addr.addr != 0) {
            return 3; // link up
        } else {
            return 2; // link no-ip;
        }
    } else {
        return phy_link_status;  // 1: link up | 0: link down
    }
}

int eth_start(eth_t *self) {
    fsp_err_t err;

    eth_lwip_deinit(self);

    do
    {
        /* When the Ethernet link status read from the PHY-LSI Basic Status register is link-up,
         * Initializes the module and make auto negotiation. */
        err = R_ETHER_LinkProcess(&g_ether0_ctrl);
    } while (FSP_SUCCESS != err);

    // while(!phy_link_status);

    eth_lwip_init(self);

    return 0;
}

int eth_stop(eth_t *self) {
    eth_lwip_deinit(self);
    return 0;
}

void eth_low_power_mode(eth_t *self, bool enable) {
    (void)self;
    printf("eth_low_power_mode() not implemented \r\n");
}

#endif // defined(MICROPY_HW_ETH_MDC)
