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

// For DHCP specs see:
//  https://www.ietf.org/rfc/rfc2131.txt
//  https://tools.ietf.org/html/rfc2132 -- DHCP Options and BOOTP Vendor Extensions

#include <stdio.h>
#include <string.h>
#include "py/mperrno.h"
#include "py/mphal.h"

#if MICROPY_PY_LWIP

#include "lib/netutils/dhcpserver.h"
#include "lwip/udp.h"

#define DHCPDISCOVER    (1)
#define DHCPOFFER       (2)
#define DHCPREQUEST     (3)
#define DHCPDECLINE     (4)
#define DHCPACK         (5)
#define DHCPNACK        (6)
#define DHCPRELEASE     (7)
#define DHCPINFORM      (8)

#define DHCP_OPT_PAD                (0)
#define DHCP_OPT_SUBNET_MASK        (1)
#define DHCP_OPT_ROUTER             (3)
#define DHCP_OPT_DNS                (6)
#define DHCP_OPT_HOST_NAME          (12)
#define DHCP_OPT_REQUESTED_IP       (50)
#define DHCP_OPT_IP_LEASE_TIME      (51)
#define DHCP_OPT_MSG_TYPE           (53)
#define DHCP_OPT_SERVER_ID          (54)
#define DHCP_OPT_PARAM_REQUEST_LIST (55)
#define DHCP_OPT_MAX_MSG_SIZE       (57)
#define DHCP_OPT_VENDOR_CLASS_ID    (60)
#define DHCP_OPT_CLIENT_ID          (61)
#define DHCP_OPT_END                (255)

#define PORT_DHCP_SERVER (67)
#define PORT_DHCP_CLIENT (68)

#define DEFAULT_DNS MAKE_IP4(8, 8, 8, 8)
#define DEFAULT_LEASE_TIME_S (24 * 60 * 60) // in seconds

#define MAC_LEN (6)
#define MAKE_IP4(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

typedef struct {
    uint8_t op; // message opcode
    uint8_t htype; // hardware address type
    uint8_t hlen; // hardware address length
    uint8_t hops;
    uint32_t xid; // transaction id, chosen by client
    uint16_t secs; // client seconds elapsed
    uint16_t flags;
    uint8_t ciaddr[4]; // client IP address
    uint8_t yiaddr[4]; // your IP address
    uint8_t siaddr[4]; // next server IP address
    uint8_t giaddr[4]; // relay agent IP address
    uint8_t chaddr[16]; // client hardware address
    uint8_t sname[64]; // server host name
    uint8_t file[128]; // boot file name
    uint8_t options[312]; // optional parameters, variable, starts with magic
} dhcp_msg_t;

static int dhcp_socket_new_dgram(struct udp_pcb **udp, void *cb_data, udp_recv_fn cb_udp_recv) {
    // family is AF_INET
    // type is SOCK_DGRAM

    *udp = udp_new();
    if (*udp == NULL) {
        return -MP_ENOMEM;
    }

    // Register callback
    udp_recv(*udp, cb_udp_recv, (void *)cb_data);

    return 0; // success
}

static void dhcp_socket_free(struct udp_pcb **udp) {
    if (*udp != NULL) {
        udp_remove(*udp);
        *udp = NULL;
    }
}

static int dhcp_socket_bind(struct udp_pcb **udp, uint32_t ip, uint16_t port) {
    ip_addr_t addr;
    IP4_ADDR(&addr, ip >> 24 & 0xff, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    // TODO convert lwIP errors to errno
    return udp_bind(*udp, &addr, port);
}

static int dhcp_socket_sendto(struct udp_pcb **udp, const void *buf, size_t len, uint32_t ip, uint16_t port) {
    if (len > 0xffff) {
        len = 0xffff;
    }

    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
    if (p == NULL) {
        return -MP_ENOMEM;
    }

    memcpy(p->payload, buf, len);

    ip_addr_t dest;
    IP4_ADDR(&dest, ip >> 24 & 0xff, ip >> 16 & 0xff, ip >> 8 & 0xff, ip & 0xff);
    err_t err = udp_sendto(*udp, p, &dest, port);

    pbuf_free(p);

    if (err != ERR_OK) {
        return err;
    }

    return len;
}

static uint8_t *opt_find(uint8_t *opt, uint8_t cmd) {
    for (int i = 0; i < 308 && opt[i] != DHCP_OPT_END;) {
        if (opt[i] == cmd) {
            return &opt[i];
        }
        i += 2 + opt[i + 1];
    }
    return NULL;
}

static void opt_write_n(uint8_t **opt, uint8_t cmd, size_t n, void *data) {
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = n;
    memcpy(o, data, n);
    *opt = o + n;
}

static void opt_write_u8(uint8_t **opt, uint8_t cmd, uint8_t val) {
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = 1;
    *o++ = val;
    *opt = o;
}

static void opt_write_u32(uint8_t **opt, uint8_t cmd, uint32_t val) {
    uint8_t *o = *opt;
    *o++ = cmd;
    *o++ = 4;
    *o++ = val >> 24;
    *o++ = val >> 16;
    *o++ = val >> 8;
    *o++ = val;
    *opt = o;
}

static void dhcp_server_process(void *arg, struct udp_pcb *upcb, struct pbuf *p, const ip_addr_t *src_addr, u16_t src_port) {
    dhcp_server_t *d = arg;
    (void)upcb;
    (void)src_addr;
    (void)src_port;

    // This is around 548 bytes
    dhcp_msg_t dhcp_msg;

    #define DHCP_MIN_SIZE (240 + 3)
    if (p->tot_len < DHCP_MIN_SIZE) {
        goto ignore_request;
    }

    size_t len = pbuf_copy_partial(p, &dhcp_msg, sizeof(dhcp_msg), 0);
    if (len < DHCP_MIN_SIZE) {
        goto ignore_request;
    }

    dhcp_msg.op = DHCPOFFER;
    memcpy(&dhcp_msg.yiaddr, &d->ip.addr, 4);

    uint8_t *opt = (uint8_t *)&dhcp_msg.options;
    opt += 4; // assume magic cookie: 99, 130, 83, 99

    switch (opt[2]) {
        case DHCPDISCOVER: {
            int yi = DHCPS_MAX_IP;
            for (int i = 0; i < DHCPS_MAX_IP; ++i) {
                if (memcmp(d->lease[i].mac, dhcp_msg.chaddr, MAC_LEN) == 0) {
                    // MAC match, use this IP address
                    yi = i;
                    break;
                }
                if (yi == DHCPS_MAX_IP) {
                    // Look for a free IP address
                    if (memcmp(d->lease[i].mac, "\x00\x00\x00\x00\x00\x00", MAC_LEN) == 0) {
                        // IP available
                        yi = i;
                    }
                    uint32_t expiry = d->lease[i].expiry << 16 | 0xffff;
                    if ((int32_t)(expiry - mp_hal_ticks_ms()) < 0) {
                        // IP expired, reuse it
                        memset(d->lease[i].mac, 0, MAC_LEN);
                        yi = i;
                    }
                }
            }
            if (yi == DHCPS_MAX_IP) {
                // No more IP addresses left
                goto ignore_request;
            }
            dhcp_msg.yiaddr[3] = DHCPS_BASE_IP + yi;
            opt_write_u8(&opt, DHCP_OPT_MSG_TYPE, DHCPOFFER);
            break;
        }

        case DHCPREQUEST: {
            uint8_t *o = opt_find(opt, DHCP_OPT_REQUESTED_IP);
            if (o == NULL) {
                // Should be NACK
                goto ignore_request;
            }
            if (memcmp(o + 2, &d->ip.addr, 3) != 0) {
                // Should be NACK
                goto ignore_request;
            }
            uint8_t yi = o[5] - DHCPS_BASE_IP;
            if (yi >= DHCPS_MAX_IP) {
                // Should be NACK
                goto ignore_request;
            }
            if (memcmp(d->lease[yi].mac, dhcp_msg.chaddr, MAC_LEN) == 0) {
                // MAC match, ok to use this IP address
            } else if (memcmp(d->lease[yi].mac, "\x00\x00\x00\x00\x00\x00", MAC_LEN) == 0) {
                // IP unused, ok to use this IP address
                memcpy(d->lease[yi].mac, dhcp_msg.chaddr, MAC_LEN);
            } else {
                // IP already in use
                // Should be NACK
                goto ignore_request;
            }
            d->lease[yi].expiry = (mp_hal_ticks_ms() + DEFAULT_LEASE_TIME_S * 1000) >> 16;
            dhcp_msg.yiaddr[3] = DHCPS_BASE_IP + yi;
            opt_write_u8(&opt, DHCP_OPT_MSG_TYPE, DHCPACK);
            printf("DHCPS: client connected: MAC=%02x:%02x:%02x:%02x:%02x:%02x IP=%u.%u.%u.%u\n",
                dhcp_msg.chaddr[0], dhcp_msg.chaddr[1], dhcp_msg.chaddr[2], dhcp_msg.chaddr[3], dhcp_msg.chaddr[4], dhcp_msg.chaddr[5],
                dhcp_msg.yiaddr[0], dhcp_msg.yiaddr[1], dhcp_msg.yiaddr[2], dhcp_msg.yiaddr[3]);
            break;
        }

        default:
            goto ignore_request;
    }

    opt_write_n(&opt, DHCP_OPT_SERVER_ID, 4, &d->ip.addr);
    opt_write_n(&opt, DHCP_OPT_SUBNET_MASK, 4, &d->nm.addr);
    opt_write_n(&opt, DHCP_OPT_ROUTER, 4, &d->ip.addr); // aka gateway; can have mulitple addresses
    opt_write_u32(&opt, DHCP_OPT_DNS, DEFAULT_DNS); // can have mulitple addresses
    opt_write_u32(&opt, DHCP_OPT_IP_LEASE_TIME, DEFAULT_LEASE_TIME_S);
    *opt++ = DHCP_OPT_END;
    dhcp_socket_sendto(&d->udp, &dhcp_msg, opt - (uint8_t *)&dhcp_msg, 0xffffffff, PORT_DHCP_CLIENT);

ignore_request:
    pbuf_free(p);
}

void dhcp_server_init(dhcp_server_t *d, ip_addr_t *ip, ip_addr_t *nm) {
    ip_addr_copy(d->ip, *ip);
    ip_addr_copy(d->nm, *nm);
    memset(d->lease, 0, sizeof(d->lease));
    if (dhcp_socket_new_dgram(&d->udp, d, dhcp_server_process) != 0) {
        return;
    }
    dhcp_socket_bind(&d->udp, 0, PORT_DHCP_SERVER);
}

void dhcp_server_deinit(dhcp_server_t *d) {
    dhcp_socket_free(&d->udp);
}

#endif // MICROPY_PY_LWIP
