/*
 * This file is part of the Micro Python project, http://micropython.org/
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
#include <errno.h>

#include "stm32f4xx_hal.h"

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#include MICROPY_HAL_H

#include "ethernet/wizchip_conf.h"
#include "ethernet/socket.h"
#include "internet/dns/dns.h"

/// \module wiznet5k - control WIZnet5x00 Ethernet adaptors
///
/// This module allows you to control WIZnet5x00 Ethernet adaptors based on
/// the W5200 and W5500 chipsets (only W5200 tested).
///
/// Example usage:
///
///     import wiznet5k
///     w = wiznet5k.WIZnet5k()
///     print(w.ipaddr())
///     w.gethostbyname('micropython.org')
///     s = w.socket()
///     s.connect(('192.168.0.2', 8080))
///     s.send('hello')
///     print(s.recv(10))

#define IPADDR_BUF_SIZE (4)

STATIC const mp_obj_type_t wiznet5k_type;
STATIC const mp_obj_type_t wiznet5k_socket_type;

STATIC mp_obj_t wiznet5k_socket_new(uint8_t sn, mp_uint_t type);

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    mp_uint_t cris_state;
    const pin_obj_t *cs;
    const pin_obj_t *rst;
    SPI_HandleTypeDef *spi;
    uint8_t socket_used;
} wiznet5k_obj_t;

STATIC wiznet5k_obj_t wiznet5k_obj;

STATIC void wiz_cris_enter(void) {
    wiznet5k_obj.cris_state = MICROPY_BEGIN_ATOMIC_SECTION();
}

STATIC void wiz_cris_exit(void) {
    MICROPY_END_ATOMIC_SECTION(wiznet5k_obj.cris_state);
}

STATIC void wiz_cs_select(void) {
    GPIO_clear_pin(wiznet5k_obj.cs->gpio, wiznet5k_obj.cs->pin_mask);
}

STATIC void wiz_cs_deselect(void) {
    GPIO_set_pin(wiznet5k_obj.cs->gpio, wiznet5k_obj.cs->pin_mask);
}

STATIC void wiz_spi_read(uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Receive(wiznet5k_obj.spi, buf, len, 5000);
    (void)status;
}

STATIC void wiz_spi_write(const uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Transmit(wiznet5k_obj.spi, (uint8_t*)buf, len, 5000);
    (void)status;
}

/******************************************************************************/
// Miscellaneous helpers

// Check the return value from Wiz socket calls:
//  - on error (<0) an exception is raised
//  - SOCK_OK or SOCK_BUSY does nothing
//  - anything positive does nothing
STATIC void check_sock_return_value(int8_t ret) {
    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "socket error %d", ret));
    }
}

STATIC void parse_ip_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    const char *addr_str = mp_obj_str_get_str(addr_in);
    const char *s = addr_str;
    for (mp_uint_t i = 0;; i++) {
        mp_uint_t val = 0;
        for (; *s && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        out_ip[i] = val;
        if (i == 3 && *s == '\0') {
            return;
        } else if (i < 3 && *s == '.') {
            s++;
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid IP address"));
        }
    }
}

// Takes an address of the form ('192.168.0.1', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
STATIC mp_uint_t parse_net_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    parse_ip_addr(addr_items[0], out_ip);
    return mp_obj_get_int(addr_items[1]);
}

// Takes an array with a raw IP address and returns something like '192.168.0.1'.
STATIC mp_obj_t format_ip_addr(uint8_t *ip) {
    char ip_str[16];
    mp_uint_t ip_len = snprintf(ip_str, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return mp_obj_new_str(ip_str, ip_len, false);
}

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('192.168.0.1', 8080).
STATIC mp_obj_t format_net_addr(uint8_t *ip, mp_uint_t port) {
    mp_obj_t tuple[2] = {
        tuple[0] = format_ip_addr(ip),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}

/******************************************************************************/
// Micro Python bindings

/// \class WIZnet5k - driver for WIZnet5x00 Ethernet modules

STATIC void wiznet5k_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    print(env, "WIZnet5k()");
}

/// \classmethod \constructor()
/// Create and return a WIZnet5k object.
STATIC mp_obj_t wiznet5k_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // init the wiznet5k object
    wiznet5k_obj.base.type = &wiznet5k_type;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.cs = &pin_B12; // Y5
    wiznet5k_obj.rst = &pin_B9; // Y4
    wiznet5k_obj.spi = &SPIHandle2; // Y-skin
    wiznet5k_obj.socket_used = 0;

    /*!< SPI configuration */
    SPIHandle2.Init.Mode = SPI_MODE_MASTER;
    SPIHandle2.Init.Direction = SPI_DIRECTION_2LINES;
    SPIHandle2.Init.DataSize = SPI_DATASIZE_8BIT;
    SPIHandle2.Init.CLKPolarity = SPI_POLARITY_LOW; // clock is low when idle
    SPIHandle2.Init.CLKPhase = SPI_PHASE_1EDGE; // data latched on first edge, which is rising edge for low-idle
    SPIHandle2.Init.NSS = SPI_NSS_SOFT;
    SPIHandle2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4; // clock freq = f_PCLK / this_prescale_value; Wiz820i can do up to 80MHz
    SPIHandle2.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPIHandle2.Init.TIMode = SPI_TIMODE_DISABLED;
    SPIHandle2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    SPIHandle2.Init.CRCPolynomial = 7; // unused
    spi_init(wiznet5k_obj.spi);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = wiznet5k_obj.cs->pin_mask;
    HAL_GPIO_Init(wiznet5k_obj.cs->gpio, &GPIO_InitStructure);

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Pin = wiznet5k_obj.rst->pin_mask;
    HAL_GPIO_Init(wiznet5k_obj.rst->gpio, &GPIO_InitStructure);

    GPIO_clear_pin(wiznet5k_obj.rst->gpio, wiznet5k_obj.rst->pin_mask);
    HAL_Delay(1); // datasheet says 2us
    GPIO_set_pin(wiznet5k_obj.rst->gpio, wiznet5k_obj.rst->pin_mask);
    HAL_Delay(160); // datasheet says 150ms

    reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
    reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
    reg_wizchip_spi_cbfunc(wiz_spi_read, wiz_spi_write);

    uint8_t sn_size[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; // 2k buffer for each socket
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    // TODO make this configurable!
    wiz_NetInfo netinfo = {
        .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
        .ip = {192, 168, 0, 18},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 0, 1},
        .dns = {8, 8, 8, 8}, // Google public DNS
        .dhcp = NETINFO_STATIC,
    };
    ctlnetwork(CN_SET_NETINFO, (void*)&netinfo);

    // return wiznet5k object
    return &wiznet5k_obj;
}

/// \method regs()
/// Dump WIZnet5k registers.
STATIC mp_obj_t wiznet5k_regs(mp_obj_t self_in) {
    //wiznet5k_obj_t *self = self_in;
    printf("Wiz CREG:");
    for (int i = 0; i < 0x50; ++i) {
        if (i % 16 == 0) {
            printf("\n  %04x:", i);
        }
        printf(" %02x", WIZCHIP_READ(i));
    }
    for (int sn = 0; sn < 4; ++sn) {
        printf("\nWiz SREG[%d]:", sn);
        for (int i = 0; i < 0x30; ++i) {
            if (i % 16 == 0) {
                printf("\n  %04x:", i);
            }
            printf(" %02x", WIZCHIP_READ(WIZCHIP_SREG_ADDR(sn, i)));
        }
    }
    printf("\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_regs_obj, wiznet5k_regs);

/// \method ipaddr([(ip, subnet, gateway, dns)])
/// Get/set IP address, subnet mask, gateway and DNS.
STATIC mp_obj_t wiznet5k_ipaddr(mp_uint_t n_args, const mp_obj_t *args) {
    wiz_NetInfo netinfo;
    ctlnetwork(CN_GET_NETINFO, &netinfo);
    if (n_args == 1) {
        // get
        mp_obj_t tuple[4] = {
            format_ip_addr(netinfo.ip),
            format_ip_addr(netinfo.sn),
            format_ip_addr(netinfo.gw),
            format_ip_addr(netinfo.dns),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        parse_ip_addr(items[0], netinfo.ip);
        parse_ip_addr(items[1], netinfo.sn);
        parse_ip_addr(items[2], netinfo.gw);
        parse_ip_addr(items[3], netinfo.dns);
        ctlnetwork(CN_SET_NETINFO, &netinfo);
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ipaddr_obj, 1, 2, wiznet5k_ipaddr);

/// \method socket(family=AF_INET, type=SOCK_STREAM, fileno=-1)
/// Create a socket.
STATIC const mp_arg_t wiznet5k_socket_args[] = {
    { MP_QSTR_family, MP_ARG_INT,                  {.u_int = 0} }, // ignored, only AF_INET supported
    { MP_QSTR_type,   MP_ARG_INT,                  {.u_int = Sn_MR_TCP} }, // SOCK_STREAM or SOCK_DGRAM
    { MP_QSTR_fileno, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = -1} },
};
#define PYB_WIZNET5K_SOCKET_NUM_ARGS MP_ARRAY_SIZE(wiznet5k_socket_args)

STATIC mp_obj_t wiznet5k_socket(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_WIZNET5K_SOCKET_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_WIZNET5K_SOCKET_NUM_ARGS, wiznet5k_socket_args, vals);

    if (vals[2].u_int < 0) {
        // get first unused socket number
        for (mp_uint_t sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++) {
            if ((wiznet5k_obj.socket_used & (1 << sn)) == 0) {
                vals[2].u_int = sn;
                break;
            }
        }
        if (vals[2].u_int < 0) {
            // too many open sockets
            nlr_raise(mp_obj_new_exception_arg1(&mp_type_OSError, MP_OBJ_NEW_SMALL_INT(EMFILE)));
        }
    }

    return wiznet5k_socket_new(vals[2].u_int, vals[1].u_int);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(wiznet5k_socket_obj, 1, wiznet5k_socket);

/// \method gethostbyname(name)
/// Use DNS to lookup a host name.  Returns an IP address.
STATIC mp_obj_t wiznet5k_gethostbyname(mp_obj_t self_in, mp_obj_t name_in) {
    uint8_t dns_ip[IPADDR_BUF_SIZE] = {8, 8, 8, 8};
    const char *name = mp_obj_str_get_str(name_in);
    uint8_t out_ip[IPADDR_BUF_SIZE];
    uint8_t *buf = m_new(uint8_t, MAX_DNS_BUF_SIZE);
    DNS_init(0, buf);
    mp_int_t ret = DNS_run(dns_ip, (uint8_t*)name, out_ip);
    m_del(uint8_t, buf, MAX_DNS_BUF_SIZE);
    if (ret == 1) {
        // success
        return format_ip_addr(out_ip);
    } else {
        // failure
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "DNS error %d", ret));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_gethostbyname_obj, wiznet5k_gethostbyname);

STATIC const mp_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_regs), (mp_obj_t)&wiznet5k_regs_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ipaddr), (mp_obj_t)&wiznet5k_ipaddr_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&wiznet5k_socket_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gethostbyname), (mp_obj_t)&wiznet5k_gethostbyname_obj },

    // class constants
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET),     MP_OBJ_NEW_SMALL_INT(0) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(Sn_MR_TCP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM),  MP_OBJ_NEW_SMALL_INT(Sn_MR_UDP) },
};

STATIC MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

STATIC const mp_obj_type_t wiznet5k_type = {
    { &mp_type_type },
    .name = MP_QSTR_WIZnet5k,
    .print = wiznet5k_print,
    .make_new = wiznet5k_make_new,
    .locals_dict = (mp_obj_t)&wiznet5k_locals_dict,
};

/******************************************************************************/
// Micro Python bindings; WIZnet5x00 socket class

typedef struct _wiznet5k_socket_obj_t {
    mp_obj_base_t base;
    uint8_t sn;
    uint8_t type;
} wiznet5k_socket_obj_t;

STATIC mp_obj_t wiznet5k_socket_new(uint8_t sn, mp_uint_t type) {
    wiznet5k_socket_obj_t *s = m_new_obj(wiznet5k_socket_obj_t);
    s->base.type = &wiznet5k_socket_type;
    s->sn = sn;
    s->type = type;
    return s;
}

STATIC void wiznet5k_socket_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    wiznet5k_socket_obj_t *self = self_in;
    print(env, "<WIZnet5k.socket sn=%u MR=0x%02x>", self->sn, getSn_MR(self->sn));
}

STATIC mp_obj_t wiznet5k_socket_close(mp_obj_t self_in) {
    wiznet5k_socket_obj_t *self = self_in;
    wiznet5k_obj.socket_used &= ~(1 << self->sn);
    mp_int_t ret = close(self->sn);
    check_sock_return_value(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_socket_close_obj, wiznet5k_socket_close);

STATIC mp_obj_t wiznet5k_socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    wiznet5k_socket_obj_t *self = self_in;

    uint8_t ip[IPADDR_BUF_SIZE];
    mp_uint_t port = parse_net_addr(addr_in, ip);

    // open the socket in server mode
    mp_int_t ret = socket(self->sn, self->type, port, 0);
    check_sock_return_value(ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_bind_obj, wiznet5k_socket_bind);

STATIC mp_obj_t wiznet5k_socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_int_t ret = listen(self->sn);
    check_sock_return_value(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_listen_obj, wiznet5k_socket_listen);

STATIC mp_obj_t wiznet5k_socket_accept(mp_obj_t self_in) {
    //wiznet5k_socket_obj_t *self = self_in;

    // TODO what to do here exactly?

    mp_obj_t tuple[2] = {self_in, mp_const_none};
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_socket_accept_obj, wiznet5k_socket_accept);

STATIC mp_obj_t wiznet5k_socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    wiznet5k_socket_obj_t *self = self_in;

    // first open the socket in client mode
    mp_int_t ret = socket(self->sn, self->type, 0, 0);
    check_sock_return_value(ret);

    // now connect
    uint8_t ip[IPADDR_BUF_SIZE];
    mp_uint_t port = parse_net_addr(addr_in, ip);
    ret = connect(self->sn, ip, port);
    check_sock_return_value(ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_connect_obj, wiznet5k_socket_connect);

STATIC mp_obj_t wiznet5k_socket_disconnect(mp_obj_t self_in) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_int_t ret = disconnect(self->sn);
    check_sock_return_value(ret);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_socket_disconnect_obj, wiznet5k_socket_disconnect);

STATIC mp_obj_t wiznet5k_socket_send(mp_obj_t self_in, mp_obj_t data_in) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    mp_int_t ret = send(self->sn, bufinfo.buf, bufinfo.len);
    check_sock_return_value(ret);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_send_obj, wiznet5k_socket_send);

STATIC mp_obj_t wiznet5k_socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_int_t len = mp_obj_get_int(len_in);
    uint8_t *buf = m_new(uint8_t, len);
    mp_int_t ret = recv(self->sn, buf, len);
    check_sock_return_value(ret);
    mp_obj_t ret_buf = mp_obj_new_bytes(buf, ret);
    m_del(uint8_t, buf, len);
    return ret_buf;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_recv_obj, wiznet5k_socket_recv);

STATIC mp_obj_t wiznet5k_socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    uint8_t ip[IPADDR_BUF_SIZE];
    mp_uint_t port = parse_net_addr(addr_in, ip);
    mp_int_t ret = sendto(self->sn, bufinfo.buf, bufinfo.len, ip, port);
    check_sock_return_value(ret);
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(wiznet5k_socket_sendto_obj, wiznet5k_socket_sendto);

STATIC mp_obj_t wiznet5k_socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    wiznet5k_socket_obj_t *self = self_in;
    mp_int_t len = mp_obj_get_int(len_in);
    uint8_t *buf = m_new(uint8_t, len);
    uint8_t ip[4];
    uint16_t port;
    mp_int_t ret = recvfrom(self->sn, buf, len, ip, &port);
    check_sock_return_value(ret);
    mp_obj_t tuple[2] = {
        mp_obj_new_bytes(buf, ret),
        format_net_addr(ip, port),
    };
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(wiznet5k_socket_recvfrom_obj, wiznet5k_socket_recvfrom);

STATIC const mp_map_elem_t wiznet5k_socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&wiznet5k_socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&wiznet5k_socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&wiznet5k_socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&wiznet5k_socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&wiznet5k_socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&wiznet5k_socket_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&wiznet5k_socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&wiznet5k_socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto), (mp_obj_t)&wiznet5k_socket_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom), (mp_obj_t)&wiznet5k_socket_recvfrom_obj },
};

STATIC MP_DEFINE_CONST_DICT(wiznet5k_socket_locals_dict, wiznet5k_socket_locals_dict_table);

STATIC const mp_obj_type_t wiznet5k_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = wiznet5k_socket_print,
    .locals_dict = (mp_obj_t)&wiznet5k_socket_locals_dict,
};

/******************************************************************************/
// Micro Python bindings; WIZnet5x00 module

STATIC const mp_map_elem_t mp_module_wiznet5k_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_wiznet5k) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WIZnet5k), (mp_obj_t)&wiznet5k_type },
};

STATIC const mp_obj_dict_t mp_module_wiznet5k_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_module_wiznet5k_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_wiznet5k_globals_table),
        .table = (mp_map_elem_t*)mp_module_wiznet5k_globals_table,
    },
};

const mp_obj_module_t mp_module_wiznet5k = {
    .base = { &mp_type_module },
    .name = MP_QSTR_wiznet5k,
    .globals = (mp_obj_dict_t*)&mp_module_wiznet5k_globals,
};
