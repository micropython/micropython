/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_NETWORK_WIZNET5K

#include "shared/netutils/netutils.h"
#include "shared/runtime/softtimer.h"
#include "extmod/modnetwork.h"
#include "extmod/modmachine.h"
#include "extmod/virtpin.h"
#include "modmachine.h"
#include "drivers/bus/spi.h"

#include "lib/wiznet5k/Ethernet/wizchip_conf.h"

// The WIZNET5K module supports two usage modes:
// * Raw ethernet transport with LwIP integrated in micropython
// * Provided TCP Stack on controller presented as modnetwork / socket interface

#define WIZNET5K_WITH_LWIP_STACK (MICROPY_PY_LWIP)
#define WIZNET5K_PROVIDED_STACK (!MICROPY_PY_LWIP)

#if WIZNET5K_WITH_LWIP_STACK
// Uses LwIP for TCP Stack with Wiznet in MACRAW mode

#include "shared/netutils/netutils.h"
#include "lib/wiznet5k/Ethernet/wizchip_conf.h"
#include "lib/wiznet5k/Ethernet/socket.h"
#include "lwip/apps/mdns.h"
#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/dhcp.h"
#include "lwip/ethip6.h"
#include "netif/etharp.h"

#define TRACE_ETH_TX (0x0002)
#define TRACE_ETH_RX (0x0004)

#else // WIZNET5K_PROVIDED_STACK
// Uses TCP Stack provided in Wiznet controller
// Requires extended socket state for ::send...
#if !MICROPY_PY_SOCKET_EXTENDED_STATE
#error WIZNET5K_PROVIDED_STACK requires MICROPY_PY_SOCKET_EXTENDED_STATE
#endif

#include "lib/wiznet5k/Ethernet/socket.h"
#include "lib/wiznet5k/Internet/DNS/dns.h"
#include "lib/wiznet5k/Internet/DHCP/dhcp.h"

// Poll WIZnet every 64ms by default (if not using interrupt pin)
#define WIZNET5K_TICK_RATE_MS 64
// In DHCP.c, RIP_MSG max size is defined as 312 + 256 (sadly, not in the header)
#define MAX_DHCP_BUF_SIZE 568

// Soft timer for polling and running DHCP in the background.
static soft_timer_entry_t mp_network_soft_timer;

#endif

#ifndef printf
#define printf(...) mp_printf(MP_PYTHON_PRINTER, __VA_ARGS__)
#endif

#ifndef MICROPY_HW_WIZNET_SPI_BAUDRATE
#define MICROPY_HW_WIZNET_SPI_BAUDRATE  (2000000)
#endif

#ifndef MICROPY_HW_WIZNET_LISTEN_BACKLOG
#define MICROPY_HW_WIZNET_LISTEN_BACKLOG 2
#endif

#ifndef WIZCHIP_SREG_ADDR
#if (_WIZCHIP_ == 5500)
#define WIZCHIP_SREG_ADDR(sn, addr)    (_W5500_IO_BASE_ + (addr << 8) + (WIZCHIP_SREG_BLOCK(sn) << 3))
#else
#define WIZCHIP_SREG_ADDR(sn, addr)    (_WIZCHIP_IO_BASE_ + WIZCHIP_SREG_BLOCK(sn) + (addr))
#endif
#endif

#if WIZNET5K_PROVIDED_STACK
typedef struct _wiznet5k_listen_wait_obj_t {
    mod_network_socket_obj_t *socket;
    uint16_t port;
} wiznet5k_listen_wait_obj_t;
#endif

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    mp_uint_t cris_state;
    mp_obj_base_t *spi;
    void (*spi_transfer)(mp_obj_base_t *obj, size_t len, const uint8_t *src, uint8_t *dest);
    mp_hal_pin_obj_t cs;
    mp_hal_pin_obj_t rst;
    mp_hal_pin_obj_t pin_intn;
    bool use_interrupt;
    #if WIZNET5K_WITH_LWIP_STACK
    uint8_t eth_frame[1514];
    uint32_t trace_flags;
    struct netif netif;
    struct dhcp dhcp_struct;
    #else // WIZNET5K_PROVIDED_STACK
    wiz_NetInfo netinfo;
    uint8_t socket_used;
    mod_network_socket_obj_t *sockets[_WIZCHIP_SOCK_NUM_];
    wiznet5k_listen_wait_obj_t listen_wait[MICROPY_HW_WIZNET_LISTEN_BACKLOG];
    bool active;
    uint8_t *dhcp_buf;
    uint8_t dhcp_state;
    mp_int_t dhcp_socket;
    uint32_t dhcp_renew;
    uint16_t tick;
    #endif
} wiznet5k_obj_t;

#if WIZNET5K_PROVIDED_STACK
typedef struct _wiznet5k_socket_extra_t {
    byte remote_ip[4];
    mp_uint_t remote_port;
} wiznet5k_socket_extra_t;
#endif

#if WIZNET5K_WITH_LWIP_STACK
#define IS_ACTIVE(self) (self->netif.flags & NETIF_FLAG_UP)
#else // WIZNET5K_PROVIDED_STACK
#define IS_ACTIVE(self) (self->active)
#endif

// Global object holding the Wiznet5k state
static wiznet5k_obj_t wiznet5k_obj;

static void wiz_cris_enter(void) {
    wiznet5k_obj.cris_state = MICROPY_BEGIN_ATOMIC_SECTION();
}

static void wiz_cris_exit(void) {
    MICROPY_END_ATOMIC_SECTION(wiznet5k_obj.cris_state);
}

static void wiz_cs_select(void) {
    mp_hal_pin_low(wiznet5k_obj.cs);
}

static void wiz_cs_deselect(void) {
    mp_hal_pin_high(wiznet5k_obj.cs);
}

void mpy_wiznet_yield(void) {
    // Used in socket.c via -DWIZCHIP_YIELD=mpy_wiznet_yield in make/cmake
    #if MICROPY_PY_THREAD
    MICROPY_THREAD_YIELD();
    #else
    mp_handle_pending(true);
    #endif
}

static void wiz_spi_read(uint8_t *buf, uint16_t len) {
    wiznet5k_obj.spi_transfer(wiznet5k_obj.spi, len, buf, buf);
}

static void wiz_spi_write(const uint8_t *buf, uint16_t len) {
    wiznet5k_obj.spi_transfer(wiznet5k_obj.spi, len, buf, NULL);
}

static uint8_t wiz_spi_readbyte() {
    uint8_t buf = 0;
    wiznet5k_obj.spi_transfer(wiznet5k_obj.spi, 1, &buf, &buf);
    return buf;
}

static void wiz_spi_writebyte(const uint8_t buf) {
    wiznet5k_obj.spi_transfer(wiznet5k_obj.spi, 1, &buf, NULL);
}

static void wiznet5k_get_mac_address(wiznet5k_obj_t *self, uint8_t mac[6]) {
    (void)self;
    getSHAR(mac);
}

void wiznet5k_try_poll(void);
static mp_obj_t mpy_wiznet_read_int(mp_obj_t none_in) {
    (void)none_in;
    // Handle incoming data, unless the SPI bus is busy
    if (mp_hal_pin_read(wiznet5k_obj.cs)) {
        wiznet5k_try_poll();
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mpy_wiznet_read_int_obj, mpy_wiznet_read_int);

static void wiznet5k_config_interrupt(bool enabled) {
    if (!wiznet5k_obj.use_interrupt) {
        return;
    }
    mp_hal_pin_interrupt(
        wiznet5k_obj.pin_intn,
        MP_OBJ_FROM_PTR(&mpy_wiznet_read_int_obj),
        (enabled)? MP_HAL_PIN_TRIGGER_FALL : MP_HAL_PIN_TRIGGER_NONE,
        true
        );
}

#if WIZNET5K_WITH_LWIP_STACK
static void wiznet5k_lwip_init(wiznet5k_obj_t *self);

void wiznet5k_deinit(void) {
    for (struct netif *netif = netif_list; netif != NULL; netif = netif->next) {
        if (netif == &wiznet5k_obj.netif) {
            #if LWIP_MDNS_RESPONDER
            mdns_resp_remove_netif(&wiznet5k_obj.netif);
            #endif
            netif_remove(netif);
            netif->flags = 0;
            break;
        }
    }
}

static void wiznet5k_init(void) {
    // Configure wiznet for raw ethernet frame usage.

    // Configure 16k buffers for fast MACRAW
    #if _WIZCHIP_ < W5200
    uint8_t sn_size[8] = {8, 0, 0, 0, 8, 0, 0, 0};
    #else
    uint8_t sn_size[16] = {16, 0, 0, 0, 0, 0, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0};
    #endif
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    if (wiznet5k_obj.use_interrupt) {
        // Enable Wiznet interrupts for socket 0
        wizchip_setinterruptmask(IK_SOCK_0);
        // Enable data receive interrupt
        setSn_IMR(0, Sn_IR_RECV);
        #if _WIZCHIP_ == W5100S
        // Enable interrupt pin
        setMR2(getMR2() | MR2_G_IEN);
        #endif

        mp_hal_pin_input(wiznet5k_obj.pin_intn);
        wiznet5k_config_interrupt(true);
    }

    // Deinit before a new init to clear the state from a previous activation
    wiznet5k_deinit();

    // Hook the Wiznet into lwIP
    wiznet5k_lwip_init(&wiznet5k_obj);

    netif_set_link_up(&wiznet5k_obj.netif);
    netif_set_up(&wiznet5k_obj.netif);

    // register with network module
    mod_network_register_nic(&wiznet5k_obj);
}

static void wiznet5k_send_ethernet(wiznet5k_obj_t *self, size_t len, const uint8_t *buf) {
    uint8_t ip[4] = {1, 1, 1, 1}; // dummy
    int ret = WIZCHIP_EXPORT(sendto)(0, (byte *)buf, len, ip, 11); // dummy port
    if (ret != len) {
        printf("wiznet5k_send_ethernet: fatal error %d\n", ret);
        netif_set_link_down(&self->netif);
        netif_set_down(&self->netif);
    }
}

// Stores the frame in self->eth_frame and returns number of bytes in the frame, 0 for no frame
static uint16_t wiznet5k_recv_ethernet(wiznet5k_obj_t *self) {
    uint16_t len = getSn_RX_RSR(0);
    if (len == 0) {
        return 0;
    }

    byte ip[4];
    uint16_t port;
    int ret = WIZCHIP_EXPORT(recvfrom)(0, self->eth_frame, 1514, ip, &port);
    if (ret <= 0) {
        printf("wiznet5k_recv_ethernet: fatal error len=%u ret=%d\n", len, ret);
        netif_set_link_down(&self->netif);
        netif_set_down(&self->netif);
        return 0;
    }

    return ret;
}

/*******************************************************************************/
// Wiznet5k lwIP interface

static err_t wiznet5k_netif_output(struct netif *netif, struct pbuf *p) {
    wiznet5k_obj_t *self = netif->state;
    pbuf_copy_partial(p, self->eth_frame, p->tot_len, 0);
    if (self->trace_flags & TRACE_ETH_TX) {
        netutils_ethernet_trace(MP_PYTHON_PRINTER, p->tot_len, self->eth_frame, NETUTILS_TRACE_IS_TX | NETUTILS_TRACE_NEWLINE);
    }
    wiznet5k_send_ethernet(self, p->tot_len, self->eth_frame);
    return ERR_OK;
}

static err_t wiznet5k_netif_init(struct netif *netif) {
    netif->linkoutput = wiznet5k_netif_output;
    netif->output = etharp_output;
    netif->mtu = 1500;
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_ETHERNET | NETIF_FLAG_IGMP;
    wiznet5k_get_mac_address(netif->state, netif->hwaddr);
    netif->hwaddr_len = sizeof(netif->hwaddr);
    int ret = WIZCHIP_EXPORT(socket)(0, Sn_MR_MACRAW, 0, 0);
    if (ret != 0) {
        printf("WIZNET fatal error in netif_init: %d\n", ret);
        return ERR_IF;
    }

    // Enable MAC filtering so we only get frames destined for us, to reduce load on lwIP
    setSn_MR(0, getSn_MR(0) | Sn_MR_MFEN);

    #if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
    netif->flags |= NETIF_FLAG_MLD6;
    #else
    // Drop IPv6 packets if firmware does not support it
    setSn_MR(0, getSn_MR(0) | Sn_MR_MIP6B);
    #endif

    return ERR_OK;
}

static void wiznet5k_lwip_init(wiznet5k_obj_t *self) {
    ip_addr_t ipconfig[4];
    IP_ADDR4(&ipconfig[0], 0, 0, 0, 0);
    IP_ADDR4(&ipconfig[1], 0, 0, 0, 0);
    IP_ADDR4(&ipconfig[2], 0, 0, 0, 0);
    IP_ADDR4(&ipconfig[3], 0, 0, 0, 0);
    netif_add(&self->netif, ip_2_ip4(&ipconfig[0]), ip_2_ip4(&ipconfig[1]), ip_2_ip4(&ipconfig[2]), self, wiznet5k_netif_init, ethernet_input);
    self->netif.name[0] = 'e';
    self->netif.name[1] = '0';
    netif_set_default(&self->netif);
    dns_setserver(0, &ipconfig[3]);
    dhcp_set_struct(&self->netif, &self->dhcp_struct);
    // Setting NETIF_FLAG_UP then clearing it is a workaround for dhcp_start and the
    // LWIP_DHCP_CHECK_LINK_UP option, so that the DHCP client schedules itself to
    // automatically start when the interface later goes up.
    self->netif.flags |= NETIF_FLAG_UP;
    dhcp_start(&self->netif);
    self->netif.flags &= ~NETIF_FLAG_UP;

    #if LWIP_MDNS_RESPONDER
    // NOTE: interface is removed in ::wiznet5k_deinit(), which is called as
    // part of the init sequence.
    mdns_resp_add_netif(&self->netif, mod_network_hostname_data);
    #endif
}

void wiznet5k_poll(void) {
    wiznet5k_obj_t *self = &wiznet5k_obj;
    if ((self->netif.flags & (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) == (NETIF_FLAG_UP | NETIF_FLAG_LINK_UP)) {
        uint16_t len;
        while ((len = wiznet5k_recv_ethernet(self)) > 0) {
            if (self->trace_flags & TRACE_ETH_RX) {
                netutils_ethernet_trace(MP_PYTHON_PRINTER, len, self->eth_frame, NETUTILS_TRACE_NEWLINE);
            }
            struct pbuf *p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
            if (p != NULL) {
                pbuf_take(p, self->eth_frame, len);
                if (self->netif.input(p, &self->netif) != ERR_OK) {
                    pbuf_free(p);
                }
            }
        }
    }
    wizchip_clrinterrupt(IK_SOCK_0);
    #if _WIZCHIP_ == W5100S
    setSn_IR(0, Sn_IR_RECV); // W5100S driver bug: must write to the Sn_IR register to reset the IRQ signal
    #endif
}

#endif // MICROPY_PY_LWIP

#if WIZNET5K_PROVIDED_STACK

static mp_int_t wiznet5k_allocate_socket(bool enable_irq) {
    // get first unused socket number
    for (mp_uint_t sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++) {
        if ((wiznet5k_obj.socket_used & (1 << sn)) == 0) {
            if (getSn_SR(sn) != SOCK_CLOSED) {
                // It's marked as unused but has not completed the state
                // transition back to close. It's safe to issue the close and
                // reuse it immediately.
                WIZCHIP_EXPORT(close)(sn);
            }
            wiznet5k_obj.socket_used |= (1 << sn);
            if (enable_irq && wiznet5k_obj.use_interrupt) {
                setSn_IMR(sn, (Sn_IR_RECV));
            }
            return sn;
        }
    }

    return -1;
}

static void wiznet5k_release_socket(mp_int_t sn) {
    wiznet5k_obj.socket_used &= ~(1 << sn);
    if (wiznet5k_obj.use_interrupt) {
        setSn_IMR(sn, 0);
        wizchip_clrinterrupt(IK_SOCK_0 << sn);
    }
}

extern uint8_t DHCP_SOCKET;
static void wiznet5k_dhcp_poll(void) {
    if (wiznet5k_obj.dhcp_socket != -1) {
        wiznet5k_obj.dhcp_state = DHCP_run();

        if (wiznet5k_obj.dhcp_state == DHCP_IP_LEASED
            || wiznet5k_obj.dhcp_state == DHCP_FAILED
            || wiznet5k_obj.dhcp_state == DHCP_STOPPED
            ) {
            // Release socket and reset timeout
            WIZCHIP_EXPORT(close)(wiznet5k_obj.dhcp_socket);
            wiznet5k_release_socket(wiznet5k_obj.dhcp_socket);
            wiznet5k_obj.dhcp_socket = -1;
        }
        // Run after about 1/8 the lease time
        if (wiznet5k_obj.dhcp_state == DHCP_IP_LEASED) {
            wiznet5k_obj.dhcp_renew = mp_hal_ticks_ms() + (getDHCPLeasetime() << 7);
        }
    } else if (wiznet5k_obj.dhcp_state == DHCP_IP_LEASED) {
        // Periodically check in about renewing the IP
        if (mp_hal_ticks_ms() > wiznet5k_obj.dhcp_renew) {
            mp_int_t sn = wiznet5k_allocate_socket(true);

            if (sn != -1) {
                DHCP_SOCKET = sn;
                wiznet5k_obj.dhcp_socket = sn;
                wiznet5k_obj.dhcp_state = DHCP_run();
            }
        }
    }
}

static bool wiznet5k_socket_try_relisten(mod_network_socket_obj_t *socket, uint16_t port, int *errno);
void wiznet5k_try_poll(void) {
    // If using DHCP for the interface, periodically renew/update the address
    wiznet5k_dhcp_poll();

    // If any callbacks are registered, poll for activity and schedule callbacks
    // to be executed.
    for (mp_uint_t sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++) {
        mod_network_socket_obj_t *socket = wiznet5k_obj.sockets[sn];
        if (socket != NULL && socket->callback != MP_OBJ_NULL) {
            uint8_t ir = getSn_IR(sn);
            if (ir & (Sn_IR_RECV | Sn_IR_CON | Sn_IR_DISCON)) {
                mp_sched_schedule(socket->callback, MP_OBJ_FROM_PTR(socket));
                setSn_IR(sn, (Sn_IR_RECV | Sn_IR_CON | Sn_IR_DISCON));
            }
        }
    }

    // If any sockets were listening and then accepted a connection, the
    // listener socket is put in the listen_wait list. Here we should try to
    // open the listener socket again.
    for (mp_uint_t sn = 0; sn < MICROPY_HW_WIZNET_LISTEN_BACKLOG; sn++) {
        wiznet5k_listen_wait_obj_t *waiter = wiznet5k_obj.listen_wait + sn;
        if (waiter->socket != NULL) {
            int _errno2;
            if (wiznet5k_socket_try_relisten(waiter->socket, waiter->port, &_errno2)) {
                // Socket is now listening again
                waiter->socket = NULL;
            }
        }
    }

    // The interrupt that triggered this will release a WFE() wait and will
    // trigger a poll() loop which will wiznet5k_socket_ioctl will detect the
    // readable or writeable state of the respective socket(s).
}

static void wiz_dhcp_assign(void) {
    getIPfromDHCP(wiznet5k_obj.netinfo.ip);
    getGWfromDHCP(wiznet5k_obj.netinfo.gw);
    getSNfromDHCP(wiznet5k_obj.netinfo.sn);
    getDNSfromDHCP(wiznet5k_obj.netinfo.dns);
    ctlnetwork(CN_SET_NETINFO, (void *)&wiznet5k_obj.netinfo);
}

static void wiz_dhcp_update(void) {
    wiz_dhcp_assign();
}


static void wiz_dhcp_conflict(void) {
    ;
}

// This is called by soft_timer and executes at PendSV level.
static void mp_network_soft_timer_callback(soft_timer_entry_t *self) {
    // For DHCP and DNS, ioLibrary expects a 1s-interval tick call
    wiznet5k_obj.tick += WIZNET5K_TICK_RATE_MS;
    if (wiznet5k_obj.tick > 1000) {
        DHCP_time_handler();
        DNS_time_handler();
        wiznet5k_obj.tick -= 1000;
    }

    wiznet5k_try_poll();
}

static void mod_network_wiznet5k_poll_init(void) {
    soft_timer_static_init(
        &mp_network_soft_timer,
        SOFT_TIMER_MODE_PERIODIC,
        WIZNET5K_TICK_RATE_MS,
        mp_network_soft_timer_callback
        );

    soft_timer_reinsert(&mp_network_soft_timer, WIZNET5K_TICK_RATE_MS);
}

static void wiznet5k_init(void) {
    // Configure wiznet provided TCP / socket interface

    reg_dhcp_cbfunc(wiz_dhcp_assign, wiz_dhcp_update, wiz_dhcp_conflict);

    uint8_t sn_size[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};         // 2k buffer for each socket
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    ctlnetwork(CN_SET_NETINFO, (void *)&wiznet5k_obj.netinfo);

    // set some sensible default values; they are configurable using ifconfig method
    wiz_NetInfo netinfo = {
        .mac = {0, 0, 0, 0, 0, 0},
        .ip = {192, 168, 0, 18},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 0, 1},
        .dns = {8, 8, 8, 8}, // Google public DNS
        .dhcp = NETINFO_STATIC,
    };
    wiznet5k_obj.netinfo = netinfo;

    if (wiznet5k_obj.use_interrupt) {
        mp_hal_pin_input(wiznet5k_obj.pin_intn);
        wiznet5k_config_interrupt(true);
        wizchip_setinterruptmask(IK_SOCK_ALL);

        #if _WIZCHIP_ == W5100S
        // Enable interrupt pin
        setMR2(getMR2() | MR2_G_IEN);
        #endif
    }

    // register with network module
    mod_network_register_nic(&wiznet5k_obj);
    mod_network_wiznet5k_poll_init();

    wiznet5k_obj.active = true;
    wiznet5k_obj.dhcp_buf = NULL;
    wiznet5k_obj.dhcp_socket = -1;
    wiznet5k_obj.dhcp_state = DHCP_STOPPED;
    wiznet5k_obj.tick = 0;
}

static int wiznet5k_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    uint8_t dns_ip[MOD_NETWORK_IPADDR_BUF_SIZE] = {8, 8, 8, 8};
    uint8_t *buf = m_new(uint8_t, MAX_DNS_BUF_SIZE);
    mp_int_t sn = wiznet5k_allocate_socket(true);
    if (sn == -1) {
        return -2;
    }

    DNS_init(sn, buf);
    if (wiznet5k_obj.netinfo.dns[0]) {
        memcpy(dns_ip, wiznet5k_obj.netinfo.dns, MOD_NETWORK_IPADDR_BUF_SIZE);
    }
    mp_int_t ret = DNS_run(dns_ip, (uint8_t *)name, out_ip);
    m_del(uint8_t, buf, MAX_DNS_BUF_SIZE);

    // NOTE: DNS_run will close the socket, so it just needs to be marked as
    // unused here and clear any interrupts.
    wiznet5k_release_socket(sn);

    if (ret == 1) {
        // success
        return 0;
    } else {
        // failure
        return -2;
    }
}

static int wiznet5k_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }

    switch (socket->type) {
        case MOD_NETWORK_SOCK_STREAM:
            socket->type = Sn_MR_TCP;
            break;
        case MOD_NETWORK_SOCK_DGRAM:
            socket->type = Sn_MR_UDP;
            break;
        default:
            *_errno = MP_EINVAL;
            return -1;
    }

    if (socket->fileno == -1) {
        // get first unused socket number
        socket->fileno = wiznet5k_allocate_socket(false);
        if (socket->fileno == -1) {
            // too many open sockets
            *_errno = MP_EMFILE;
            return -1;
        }

        socket->_private = NULL;
        socket->callback = MP_OBJ_NULL;
        wiznet5k_obj.sockets[socket->fileno] = socket;

        // Enable data receive interrupt
        if (wiznet5k_obj.use_interrupt) {
            setSn_IMR(socket->fileno, (Sn_IR_RECV | Sn_IR_CON | Sn_IR_DISCON));
        }
    }

    // WIZNET does not have a concept of pure "open socket".  You need to know
    // if it's a server or client at the time of creation of the socket.
    // So, we defer the open until we know what kind of socket we want.

    return 0;
}

static void wiznet5k_socket_close(mod_network_socket_obj_t *socket) {
    uint8_t sn = (uint8_t)socket->fileno;
    if (sn < _WIZCHIP_SOCK_NUM_) {
        uint8_t status = getSn_SR((uint8_t)socket->fileno);
        if (status == SOCK_ESTABLISHED || status == SOCK_CLOSE_WAIT) {
            // Send a FIN packet to the remote
            MP_THREAD_GIL_EXIT();
            WIZCHIP_EXPORT(disconnect)(sn);
            MP_THREAD_GIL_ENTER();
        } else {
            WIZCHIP_EXPORT(close)(sn);
        }
        wiznet5k_release_socket(sn);
        wiznet5k_obj.sockets[sn] = NULL;
    }

    for (sn = 0; sn < MICROPY_HW_WIZNET_LISTEN_BACKLOG; sn++) {
        wiznet5k_listen_wait_obj_t *waiter = wiznet5k_obj.listen_wait + sn;
        if (waiter->socket == socket) {
            // Cancel the wait for an open socket
            waiter->socket = NULL;
            break;
        }
    }

    socket->fileno = -1;
    if (socket->_private != NULL) {
        m_del(wiznet5k_socket_extra_t, socket->_private, 1);
        socket->_private = NULL;
    }
}

static int wiznet5k_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    uint8_t flag = 0;
    if (socket->timeout == 0) {
        // Setup non-blocking mode
        flag |= SOCK_IO_NONBLOCK;
    }

    if (getSn_MR(socket->fileno) & Sn_MR_ND) {
        flag |= SF_TCP_NODELAY;
    }

    // open the socket in server mode (if port != 0)
    mp_int_t ret = WIZCHIP_EXPORT(socket)(socket->fileno, socket->type, port, flag);

    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }

    // indicate that this socket has been opened
    socket->bound = true;

    // success
    return 0;
}

static int wiznet5k_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    mp_int_t ret = WIZCHIP_EXPORT(listen)(socket->fileno);
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return 0;
}

static bool wiznet5k_socket_try_relisten(mod_network_socket_obj_t *socket, uint16_t port, int *_errno) {
    socket->fileno = -1;
    socket->bound = false;

    if (wiznet5k_socket_socket(socket, _errno) == 0
        && wiznet5k_socket_bind(socket, NULL, port, _errno) == 0
        && wiznet5k_socket_listen(socket, 0, _errno) == 0) {
        return true;
    }

    // TODO: Inspect for terminal ERRNO and return true to indicate listen
    // should not be retried.
    return false;
}

static void wiznet5k_socket_relisten(mod_network_socket_obj_t *socket, uint16_t port) {
    int _errno2;

    if (!wiznet5k_socket_try_relisten(socket, port, &_errno2)) {
        // Unable to re-listen now (probably no more available sockets).
        // Instead, add to a wait list and re-listen when a socket
        // becomes available.
        for (mp_uint_t sn = 0; sn < MICROPY_HW_WIZNET_LISTEN_BACKLOG; sn++) {
            wiznet5k_listen_wait_obj_t *waiter = wiznet5k_obj.listen_wait + sn;
            if (waiter->socket == NULL) {
                waiter->port = port;
                waiter->socket = socket;
                break;
            }
        }
    }
}

static int wiznet5k_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    uint8_t sn = (uint8_t)socket->fileno;
    if (sn >= _WIZCHIP_SOCK_NUM_) {
        *_errno = MP_EINVAL;
        return -1;
    }

    mp_uint_t start = mp_hal_ticks_ms();
    for (;;) {
        int sr = getSn_SR((uint8_t)socket->fileno);
        if (sr == SOCK_ESTABLISHED) {
            socket2->domain = socket->domain;
            socket2->type = socket->type;
            socket2->fileno = socket->fileno;
            getSn_DIPR((uint8_t)socket2->fileno, ip);
            *port = getSn_DPORT(socket2->fileno);

            // The socket number here didn't change, but the socket did, so
            // update the list of open sockets.
            wiznet5k_obj.sockets[socket2->fileno] = socket2;

            // WIZnet turns the listening socket into the client socket, so we
            // need to re-bind and re-listen on another socket for the server.
            wiznet5k_socket_relisten(socket, getSn_PORT(socket2->fileno));

            return 0;
        }
        if (sr == SOCK_CLOSED || sr == SOCK_CLOSE_WAIT) {
            // Somehow the accepted socket was closed. Attempt to start listening again
            uint16_t port = getSn_PORT(socket->fileno);
            wiznet5k_socket_close(socket);
            wiznet5k_socket_relisten(socket, port);
            *_errno = MP_ENOTCONN; // ??
            return -1;
        }
        if (socket->timeout == 0) {
            *_errno = MP_EAGAIN;
            return -1;
        } else if (socket->timeout > 0) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) > (uint32_t)socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
        }
        mp_event_wait_ms(1);
    }
}

static int wiznet5k_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    // use "bind" function to open the socket in client mode
    if (wiznet5k_socket_bind(socket, ip, 0, _errno) != 0) {
        return -1;
    }

    // WIZnet doesn't support connect on UDP sockets. Stash the remote
    // information for use with the ::send method, if used on this UDP socket.
    if (socket->type == Sn_MR_TCP) {
        // now connect
        MP_THREAD_GIL_EXIT();
        mp_int_t ret = WIZCHIP_EXPORT(connect)(socket->fileno, ip, port);
        MP_THREAD_GIL_ENTER();

        if (ret < 0) {
            wiznet5k_socket_close(socket);
            *_errno = -ret;
            return -1;
        } else if (ret == SOCK_BUSY) {
            *_errno = MP_EAGAIN;
            return -1;
        }
    } else if (socket->type == Sn_MR_UDP) {
        // For POSIX usage of ::send later, stash the remote IP and port
        wiznet5k_socket_extra_t *extra = m_new_maybe(wiznet5k_socket_extra_t, 1);
        if (extra == NULL) {
            *_errno = MP_ENOMEM;
            return -1;
        }
        memcpy(extra->remote_ip, ip, 4);
        extra->remote_port = port;
        socket->_private = extra;
    }

    // success
    return 0;
}

static mp_uint_t wiznet5k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno);
static mp_uint_t wiznet5k_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    if (socket->type == Sn_MR_UDP) {
        if (socket->_private != NULL) {
            wiznet5k_socket_extra_t *extra = (wiznet5k_socket_extra_t *)socket->_private;
            return wiznet5k_socket_sendto(socket, buf, len, extra->remote_ip, extra->remote_port, _errno);
        }
        *_errno = MP_ENOTCONN;
        return -1;
    }

    if (socket->timeout > 0) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (getSn_SR(socket->fileno) == SOCK_ESTABLISHED && getSn_TX_FSR(socket->fileno) == 0) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) > (uint32_t)socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            mp_event_wait_ms(1);
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(send)(socket->fileno, (byte *)buf, len);
    MP_THREAD_GIL_ENTER();

    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    } else if (ret == SOCK_BUSY) {
        uint8_t status = getSn_SR(socket->fileno);
        if (status == SOCK_ESTABLISHED || status == SOCK_CLOSE_WAIT) {
            *_errno = MP_EAGAIN;
            return -1;
        }
    }
    return ret;
}

static mp_uint_t wiznet5k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno);
static mp_uint_t wiznet5k_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    if (socket->type == Sn_MR_UDP) {
        byte remote_ip[4];
        mp_uint_t remote_port;
        return wiznet5k_socket_recvfrom(socket, buf, len, remote_ip, &remote_port, _errno);
    }

    if (socket->timeout > 0) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (getSn_SR(socket->fileno) == SOCK_ESTABLISHED && getSn_RX_RSR(socket->fileno) == 0) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) > (uint32_t)socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            mp_event_wait_ms(1);
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(recv)(socket->fileno, buf, len);
    MP_THREAD_GIL_ENTER();

    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    } else if (ret == SOCK_BUSY) {
        // NOTE: SOCK_BUSY is zero (0) which is confusing if the socket is closed
        // and at EOF
        uint8_t status = getSn_SR(socket->fileno);
        if (status == SOCK_ESTABLISHED || status == SOCK_CLOSE_WAIT) {
            *_errno = MP_EAGAIN;
            return -1;
        }
    }
    return ret;
}

static mp_uint_t wiznet5k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    if (!socket->bound) {
        // socket not opened; use "bind" function to open the socket in client mode
        if (wiznet5k_socket_bind(socket, ip, 0, _errno) != 0) {
            return -1;
        }
    }

    if (socket->timeout > 0) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (getSn_SR(socket->fileno) != SOCK_CLOSED && getSn_TX_FSR(socket->fileno) == 0) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) > (uint32_t)socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            mp_event_wait_ms(1);
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(sendto)(socket->fileno, (byte *)buf, len, ip, port);
    MP_THREAD_GIL_ENTER();

    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    } else if (ret == SOCK_BUSY) {
        *_errno = MP_EAGAIN;
        return -1;
    }
    return ret;
}

static mp_uint_t wiznet5k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    uint16_t port2;

    if (socket->timeout > 0) {
        mp_uint_t start = mp_hal_ticks_ms();
        while (getSn_SR(socket->fileno) != SOCK_CLOSED && getSn_RX_RSR(socket->fileno) == 0) {
            if ((uint32_t)(mp_hal_ticks_ms() - start) > (uint32_t)socket->timeout) {
                *_errno = MP_ETIMEDOUT;
                return -1;
            }
            mp_event_wait_ms(1);
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(recvfrom)(socket->fileno, buf, len, ip, &port2);
    MP_THREAD_GIL_ENTER();
    *port = port2;
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    } else if (ret == SOCK_BUSY) {
        *_errno = MP_EAGAIN;
        return -1;
    }
    return ret;
}

static int wiznet5k_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    switch (opt) {
        case 20:
            // User callback on receive
            socket->callback = (void *)optval;
            break;

        // level: SOL_SOCKET
        case MOD_NETWORK_SO_REUSEADDR:
        case MOD_NETWORK_SO_BROADCAST:
            // Implied/not-required in Wiznet sockets
            break;

        // level: IPPROTO_IP

        // level: IPPROTO_TCP
        case MOD_NETWORK_TCP_NODELAY: {
            mp_int_t val = (mp_int_t)optval;
            if (val) {
                setSn_MR(socket->fileno, getSn_MR(socket->fileno) | Sn_MR_ND);
            } else {
                setSn_MR(socket->fileno, getSn_MR(socket->fileno) & ~Sn_MR_ND);
            }
            break;
        }

        default:
            *_errno = MP_EINVAL;
            return -1;
    }

    return 0;
}

static int wiznet5k_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    uint8_t arg;
    if (timeout_ms == 0) {
        // set non-blocking mode
        arg = SOCK_IO_NONBLOCK;
    } else {
        arg = SOCK_IO_BLOCK;
    }

    mp_int_t ret = WIZCHIP_EXPORT(ctlsocket)(socket->fileno, CS_SET_IOMODE, &arg);
    if (ret < 0) {
        *_errno = -ret;
        return -1;
    }

    socket->timeout = timeout_ms;
    return 0;
}

static int wiznet5k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    uint8_t sn = (uint8_t)socket->fileno;

    if (request == MP_STREAM_POLL) {
        int ret = 0;
        if (sn < _WIZCHIP_SOCK_NUM_) {
            if (arg & MP_STREAM_POLL_RD && getSn_RX_RSR(sn) != 0) {
                ret |= MP_STREAM_POLL_RD;
            }
            if (arg & MP_STREAM_POLL_WR && getSn_TX_FSR(sn) != 0) {
                ret |= MP_STREAM_POLL_WR;
            }

            uint8_t status = getSn_SR(sn);
            if (status == SOCK_CLOSE_WAIT || getSn_IR(sn) & Sn_IR_DISCON) {
                // Peer-closed socket is both readable and writable: read will
                // return EOF, write - error. Without this poll will hang on a
                // socket which was closed by peer.
                ret |= arg & (MP_STREAM_POLL_RD | MP_STREAM_POLL_WR);
            } else if (status == SOCK_CLOSED) {
                ret |= MP_STREAM_POLL_ERR;
            }

            if (wiznet5k_obj.use_interrupt) {
                setSn_IR(sn, (Sn_IR_RECV | Sn_IR_CON | Sn_IR_DISCON));
            }
        } else if (socket->state == MOD_NETWORK_SS_LISTENING) {
            // Listening socket is in dormant state and not currently listening,
            // so also not currently readable or writeable.
            ret = 0;
        } else {
            ret |= MP_STREAM_POLL_NVAL;
        }

        return ret;
    } else {
        *_errno = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

static void wiznet5k_dhcp_init(wiznet5k_obj_t *self) {
    uint8_t dhcp_retry = 0;
    self->dhcp_state = DHCP_STOPPED;
    if (self->dhcp_buf == NULL) {
        self->dhcp_buf = m_new_maybe(uint8_t, MAX_DHCP_BUF_SIZE);
        if (self->dhcp_buf == NULL) {
            return;
        }
    }

    if (self->dhcp_socket == -1) {
        self->dhcp_socket = wiznet5k_allocate_socket(true);
        if (self->dhcp_socket == -1) {
            m_del(uint8_t, self->dhcp_buf, MAX_DHCP_BUF_SIZE);
            return;
        }
    }

    while (self->dhcp_state != DHCP_IP_LEASED) {
        mp_uint_t timeout = mp_hal_ticks_ms() + 3000;
        DHCP_init(self->dhcp_socket, self->dhcp_buf);

        while (1) {
            mp_event_wait_ms(1);
            if (self->dhcp_state == DHCP_IP_LEASED) {
                break;
            } else if (self->dhcp_state == DHCP_FAILED || mp_hal_ticks_ms() > timeout) {
                dhcp_retry++;
                break;
            }
        }

        if (dhcp_retry > 3) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("timeout waiting for DHCP to get IP address"));
            break;
        }
    }

    if (self->dhcp_state == DHCP_IP_LEASED) {
        ctlnetwork(CN_GET_NETINFO, &self->netinfo);
    }
}

#endif // WIZNET5K_PROVIDED_STACK

/*******************************************************************************/
// MicroPython bindings

// WIZNET5K(spi, pin_cs, pin_rst[, pin_intn])
// Create and return a WIZNET5K object.
static mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_obj_base_t *spi;
    mp_hal_pin_obj_t cs;
    mp_hal_pin_obj_t rst;
    mp_hal_pin_obj_t pin_intn = (mp_hal_pin_obj_t)NULL;
    bool use_interrupt = false;

    #if WIZNET5K_WITH_LWIP_STACK
    // Bring down interface while configuring
    wiznet5k_obj.netif.flags = 0;
    #endif

    #ifdef MICROPY_HW_WIZNET_SPI_ID
    // Allow auto-configuration of SPI if defined for board and no args passed
    if (n_args == 0 && n_kw == 0) {
        // Initialize SPI.
        mp_obj_t spi_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_SPI_SCK);
        mp_obj_t miso_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_SPI_MISO);
        mp_obj_t mosi_obj = MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_SPI_MOSI);
        mp_obj_t args[] = {
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_SPI_ID),
            MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_SPI_BAUDRATE),
            MP_ROM_QSTR(MP_QSTR_sck), mp_pin_make_new(NULL, 1, 0, &spi_obj),
            MP_ROM_QSTR(MP_QSTR_miso), mp_pin_make_new(NULL, 1, 0, &miso_obj),
            MP_ROM_QSTR(MP_QSTR_mosi), mp_pin_make_new(NULL, 1, 0, &mosi_obj),
        };
        spi = MP_OBJ_TO_PTR(MP_OBJ_TYPE_GET_SLOT(&machine_spi_type, make_new)((mp_obj_t)&machine_spi_type, 2, 3, args));

        cs = mp_hal_get_pin_obj(mp_pin_make_new(NULL, 1, 0, (mp_obj_t[]) {MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_PIN_CS)}));
        rst = mp_hal_get_pin_obj(mp_pin_make_new(NULL, 1, 0, (mp_obj_t[]) {MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_PIN_RST)}));
        #ifdef MICROPY_HW_WIZNET_PIN_INTN
        pin_intn = mp_hal_get_pin_obj(mp_pin_make_new(NULL, 1, 0, (mp_obj_t[]) {MP_OBJ_NEW_SMALL_INT(MICROPY_HW_WIZNET_PIN_INTN)}));
        use_interrupt = true;
        #endif

    } else
    #endif
    {
        // If passing in args, must supply spi, pin_cs, pin_rst and optionally pin_intn
        mp_arg_check_num(n_args, n_kw, 3, 4, false);
        spi = mp_hal_get_spi_obj(args[0]);
        cs = mp_hal_get_pin_obj(args[1]);
        rst = mp_hal_get_pin_obj(args[2]);
        if (n_args > 3) {
            pin_intn = mp_hal_get_pin_obj(args[3]);
            use_interrupt = true;
        }
    }

    mp_hal_pin_output(cs);
    mp_hal_pin_output(rst);

    // Init the wiznet5k object
    wiznet5k_obj.base.type = (mp_obj_type_t *)&mod_network_nic_type_wiznet5k;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.spi = spi;
    wiznet5k_obj.spi_transfer = ((mp_machine_spi_p_t *)MP_OBJ_TYPE_GET_SLOT(spi->type, protocol))->transfer;
    wiznet5k_obj.cs = cs;
    wiznet5k_obj.rst = rst;
    wiznet5k_obj.pin_intn = pin_intn;
    wiznet5k_obj.use_interrupt = use_interrupt;
    #if WIZNET5K_WITH_LWIP_STACK
    wiznet5k_obj.trace_flags = 0;
    #else // WIZNET5K_PROVIDED_STACK
    wiznet5k_obj.active = false;
    wiznet5k_obj.socket_used = 0;
    memset(wiznet5k_obj.sockets, 0, sizeof(wiznet5k_obj.sockets));
    memset(wiznet5k_obj.listen_wait, 0, sizeof(wiznet5k_obj.listen_wait));
    #endif

    // Return wiznet5k object
    return MP_OBJ_FROM_PTR(&wiznet5k_obj);
}

// regs()
// Dump WIZNET5K registers.
static mp_obj_t wiznet5k_regs(mp_obj_t self_in) {
    (void)self_in;
    printf("Wiz CREG:");
    for (int i = 0; i < 0x50; ++i) {
        if (i % 16 == 0) {
            printf("\n  %04x:", i);
        }
        #if _WIZCHIP_ == 5500
        uint32_t reg = _W5500_IO_BASE_ | i << 8;
        #else
        uint32_t reg = i;
        #endif
        printf(" %02x", WIZCHIP_READ(reg));
    }
    for (int sn = 0; sn < 4; ++sn) {
        printf("\nWiz SREG[%d]:", sn);
        for (int i = 0; i < 0x30; ++i) {
            if (i % 16 == 0) {
                printf("\n  %04x:", i);
            }
            #if _WIZCHIP_ == 5500
            uint32_t reg = _W5500_IO_BASE_ | i << 8 | WIZCHIP_SREG_BLOCK(sn) << 3;
            #else
            uint32_t reg = WIZCHIP_SREG_ADDR(sn, i);
            #endif
            printf(" %02x", WIZCHIP_READ(reg));
        }
    }
    printf("\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_regs_obj, wiznet5k_regs);

static mp_obj_t wiznet5k_isconnected(mp_obj_t self_in) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(
        wizphy_getphylink() == PHY_LINK_ON
        && IS_ACTIVE(self)
        #if WIZNET5K_WITH_LWIP_STACK
        && ip_2_ip4(&self->netif.ip_addr)->addr != 0
        #endif
        );
}
static MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_isconnected_obj, wiznet5k_isconnected);

static mp_obj_t wiznet5k_active(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        return mp_obj_new_bool(IS_ACTIVE(self));
    } else {
        if (mp_obj_is_true(args[1])) {
            if (!IS_ACTIVE(self)) {
                /*!< Wiznet initialisation */
                // Reset the chip
                mp_hal_pin_low(wiznet5k_obj.rst);
                mp_hal_delay_ms(1); // datasheet says 2us
                mp_hal_pin_high(wiznet5k_obj.rst);
                mp_hal_delay_ms(160); // datasheet says 150ms

                // Set physical interface callbacks
                reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
                reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
                reg_wizchip_spi_cbfunc(wiz_spi_readbyte, wiz_spi_writebyte);
                reg_wizchip_spiburst_cbfunc(wiz_spi_read, wiz_spi_write);

                // Configure lwip/provided specific settings
                wiznet5k_init();

                // If the device doesn't have a MAC address then set one
                #if WIZNET5K_WITH_LWIP_STACK
                uint8_t *mac = self->netif.hwaddr;
                #else // WIZNET5K_PROVIDED_STACK
                uint8_t *mac = wiznet5k_obj.netinfo.mac;
                #endif
                getSHAR(mac);
                if ((mac[0] | mac[1] | mac[2] | mac[3] | mac[4] | mac[5]) == 0) {
                    mp_hal_get_mac(MP_HAL_MAC_ETH0, mac);
                    setSHAR(mac);
                }

                #if WIZNET5K_WITH_LWIP_STACK && LWIP_IPV6
                netif_create_ip6_linklocal_address(&self->netif, 1);
                #endif

                // seems we need a small delay after init
                mp_hal_delay_ms(250);

            }
        } else {
            #if WIZNET5K_WITH_LWIP_STACK
            netif_set_down(&self->netif);
            netif_set_link_down(&self->netif);
            wiznet5k_deinit();
            #else // WIZNET5K_PROVIDED_STACK
            self->active = false;
            wizchip_sw_reset();
            #endif
        }
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_active_obj, 1, 2, wiznet5k_active);

#if WIZNET5K_PROVIDED_STACK
// ifconfig([(ip, subnet, gateway, dns)])
// Get/set IP address, subnet mask, gateway and DNS.
static mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiz_NetInfo netinfo;
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    ctlnetwork(CN_GET_NETINFO, &netinfo);
    if (n_args == 1) {
        // Get IP addresses
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr(netinfo.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.sn, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else if (args[1] == MP_OBJ_NEW_QSTR(MP_QSTR_dhcp)) {
        // Start the DHCP client
        self->netinfo.dhcp = NETINFO_DHCP;
        wiznet5k_dhcp_init((void *)self);
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr(self->netinfo.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr(self->netinfo.sn, NETUTILS_BIG),
            netutils_format_ipv4_addr(self->netinfo.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr(self->netinfo.dns, NETUTILS_BIG),

        };
        return mp_obj_new_tuple(4, tuple);

    } else {
        // Set static IP addresses
        self->netinfo.dhcp = NETINFO_STATIC;
        self->dhcp_state = DHCP_STOPPED;
        if (self->dhcp_buf) {
            m_del(uint8_t, self->dhcp_buf, MAX_DHCP_BUF_SIZE);
            self->dhcp_buf = NULL;
        }
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[0], netinfo.ip, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[1], netinfo.sn, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[2], netinfo.gw, NETUTILS_BIG);
        netutils_parse_ipv4_addr(items[3], netinfo.dns, NETUTILS_BIG);
        ctlnetwork(CN_SET_NETINFO, &netinfo);
        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);
#endif // WIZNET5K_PROVIDED_STACK

#if WIZNET5K_WITH_LWIP_STACK

static mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->netif, n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);

static mp_obj_t network_wiznet5k_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->netif, n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(wiznet5k_ipconfig_obj, 1, network_wiznet5k_ipconfig);

static mp_obj_t send_ethernet_wrapper(mp_obj_t self_in, mp_obj_t buf_in) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_buffer_info_t buf;
    mp_get_buffer_raise(buf_in, &buf, MP_BUFFER_READ);
    wiznet5k_send_ethernet(self, buf.len, buf.buf);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_2(send_ethernet_obj, send_ethernet_wrapper);

#endif // MICROPY_PY_LWIP

static mp_obj_t wiznet5k_status(size_t n_args, const mp_obj_t *args) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    (void)self;

    if (n_args == 1) {
        // No arguments: return link status
        if (wizphy_getphylink() == PHY_LINK_ON) {
            if (IS_ACTIVE(self)) {
                return MP_OBJ_NEW_SMALL_INT(2);
            } else {
                return MP_OBJ_NEW_SMALL_INT(1);
            }
        } else {
            return MP_OBJ_NEW_SMALL_INT(0);
        }
    }
    mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_status_obj, 1, 2, wiznet5k_status);

static mp_obj_t wiznet5k_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    wiznet5k_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used == 0) {
        // Get config value
        if (n_args != 2) {
            mp_raise_TypeError(MP_ERROR_TEXT("must query one param"));
        }

        switch (mp_obj_str_get_qstr(args[1])) {
            case MP_QSTR_mac: {
                uint8_t buf[6];
                wiznet5k_get_mac_address(self, buf);
                return mp_obj_new_bytes(buf, 6);
            }
            default:
                mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
        }
    } else {
        // Set config value(s)
        if (n_args != 1) {
            mp_raise_TypeError(MP_ERROR_TEXT("can't specify pos and kw args"));
        }

        for (size_t i = 0; i < kwargs->alloc; ++i) {
            if (MP_MAP_SLOT_IS_FILLED(kwargs, i)) {
                mp_map_elem_t *e = &kwargs->table[i];
                switch (mp_obj_str_get_qstr(e->key)) {
                    case MP_QSTR_mac: {
                        mp_buffer_info_t buf;
                        mp_get_buffer_raise(e->value, &buf, MP_BUFFER_READ);
                        if (buf.len != 6) {
                            mp_raise_ValueError(NULL);
                        }
                        setSHAR(buf.buf);
                        #if WIZNET5K_WITH_LWIP_STACK
                        memcpy(self->netif.hwaddr, buf.buf, 6);
                        #endif
                        break;
                    }
                    #if WIZNET5K_WITH_LWIP_STACK
                    case MP_QSTR_trace: {
                        self->trace_flags = mp_obj_get_int(e->value);
                        break;
                    }
                    #endif
                    default:
                        mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
                }
            }
        }

        return mp_const_none;
    }
}
static MP_DEFINE_CONST_FUN_OBJ_KW(wiznet5k_config_obj, 1, wiznet5k_config);

static const mp_rom_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_regs), MP_ROM_PTR(&wiznet5k_regs_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wiznet5k_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&wiznet5k_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wiznet5k_ifconfig_obj) },
    #if WIZNET5K_WITH_LWIP_STACK
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&wiznet5k_ipconfig_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&wiznet5k_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&wiznet5k_config_obj) },
    #if WIZNET5K_WITH_LWIP_STACK
    { MP_ROM_QSTR(MP_QSTR_send_ethernet), MP_ROM_PTR(&send_ethernet_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

#if WIZNET5K_WITH_LWIP_STACK
#define NIC_TYPE_WIZNET_PROTOCOL
#else // WIZNET5K_PROVIDED_STACK
const mod_network_nic_protocol_t mod_network_nic_protocol_wiznet = {
    .gethostbyname = wiznet5k_gethostbyname,
    .socket = wiznet5k_socket_socket,
    .close = wiznet5k_socket_close,
    .bind = wiznet5k_socket_bind,
    .listen = wiznet5k_socket_listen,
    .accept = wiznet5k_socket_accept,
    .connect = wiznet5k_socket_connect,
    .send = wiznet5k_socket_send,
    .recv = wiznet5k_socket_recv,
    .sendto = wiznet5k_socket_sendto,
    .recvfrom = wiznet5k_socket_recvfrom,
    .setsockopt = wiznet5k_socket_setsockopt,
    .settimeout = wiznet5k_socket_settimeout,
    .ioctl = wiznet5k_socket_ioctl,
};
#define NIC_TYPE_WIZNET_PROTOCOL protocol, &mod_network_nic_protocol_wiznet,
#endif

MP_DEFINE_CONST_OBJ_TYPE(
    mod_network_nic_type_wiznet5k,
    MP_QSTR_WIZNET5K,
    MP_TYPE_FLAG_NONE,
    make_new, wiznet5k_make_new,
    NIC_TYPE_WIZNET_PROTOCOL
    locals_dict, &wiznet5k_locals_dict
    );

#endif // MICROPY_PY_NETWORK_WIZNET5K
