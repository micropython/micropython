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

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "modnetwork.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"

#include "ethernet/wizchip_conf.h"
#include "ethernet/socket.h"
#include "internet/dns/dns.h"

/// \moduleref network

typedef struct _wiznet5k_obj_t {
    mp_obj_base_t base;
    mp_uint_t cris_state;
    SPI_HandleTypeDef *spi;
    const pin_obj_t *cs;
    const pin_obj_t *rst;
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
    mp_hal_pin_low(wiznet5k_obj.cs);
}

STATIC void wiz_cs_deselect(void) {
    mp_hal_pin_high(wiznet5k_obj.cs);
}

STATIC void wiz_spi_read(uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Receive(wiznet5k_obj.spi, buf, len, 5000);
    (void)status;
}

STATIC void wiz_spi_write(const uint8_t *buf, uint32_t len) {
    HAL_StatusTypeDef status = HAL_SPI_Transmit(wiznet5k_obj.spi, (uint8_t*)buf, len, 5000);
    (void)status;
}

STATIC int wiznet5k_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    uint8_t dns_ip[MOD_NETWORK_IPADDR_BUF_SIZE] = {8, 8, 8, 8};
    uint8_t *buf = m_new(uint8_t, MAX_DNS_BUF_SIZE);
    DNS_init(0, buf);
    mp_int_t ret = DNS_run(dns_ip, (uint8_t*)name, out_ip);
    m_del(uint8_t, buf, MAX_DNS_BUF_SIZE);
    if (ret == 1) {
        // success
        return 0;
    } else {
        // failure
        return -2;
    }
}

STATIC int wiznet5k_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->u_param.domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }

    switch (socket->u_param.type) {
        case MOD_NETWORK_SOCK_STREAM: socket->u_param.type = Sn_MR_TCP; break;
        case MOD_NETWORK_SOCK_DGRAM: socket->u_param.type = Sn_MR_UDP; break;
        default: *_errno = MP_EINVAL; return -1;
    }

    if (socket->u_param.fileno == -1) {
        // get first unused socket number
        for (mp_uint_t sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++) {
            if ((wiznet5k_obj.socket_used & (1 << sn)) == 0) {
                wiznet5k_obj.socket_used |= (1 << sn);
                socket->u_param.fileno = sn;
                break;
            }
        }
        if (socket->u_param.fileno == -1) {
            // too many open sockets
            *_errno = MP_EMFILE;
            return -1;
        }
    }

    // WIZNET does not have a concept of pure "open socket".  You need to know
    // if it's a server or client at the time of creation of the socket.
    // So, we defer the open until we know what kind of socket we want.

    // use "domain" to indicate that this socket has not yet been opened
    socket->u_param.domain = 0;

    return 0;
}

STATIC void wiznet5k_socket_close(mod_network_socket_obj_t *socket) {
    uint8_t sn = (uint8_t)socket->u_param.fileno;
    if (sn < _WIZCHIP_SOCK_NUM_) {
        wiznet5k_obj.socket_used &= ~(1 << sn);
        WIZCHIP_EXPORT(close)(sn);
    }
}

STATIC int wiznet5k_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    // open the socket in server mode (if port != 0)
    mp_int_t ret = WIZCHIP_EXPORT(socket)(socket->u_param.fileno, socket->u_param.type, port, 0);
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }

    // indicate that this socket has been opened
    socket->u_param.domain = 1;

    // success
    return 0;
}

STATIC int wiznet5k_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    mp_int_t ret = WIZCHIP_EXPORT(listen)(socket->u_param.fileno);
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return 0;
}

STATIC int wiznet5k_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    for (;;) {
        int sr = getSn_SR((uint8_t)socket->u_param.fileno);
        if (sr == SOCK_ESTABLISHED) {
            socket2->u_param = socket->u_param;
            getSn_DIPR((uint8_t)socket2->u_param.fileno, ip);
            *port = getSn_PORT(socket2->u_param.fileno);

            // WIZnet turns the listening socket into the client socket, so we
            // need to re-bind and re-listen on another socket for the server.
            // TODO handle errors, especially no-more-sockets error
            socket->u_param.domain = MOD_NETWORK_AF_INET;
            socket->u_param.fileno = -1;
            int _errno2;
            if (wiznet5k_socket_socket(socket, &_errno2) != 0) {
                //printf("(bad resocket %d)\n", _errno2);
            } else if (wiznet5k_socket_bind(socket, NULL, *port, &_errno2) != 0) {
                //printf("(bad rebind %d)\n", _errno2);
            } else if (wiznet5k_socket_listen(socket, 0, &_errno2) != 0) {
                //printf("(bad relisten %d)\n", _errno2);
            }

            return 0;
        }
        if (sr == SOCK_CLOSED || sr == SOCK_CLOSE_WAIT) {
            wiznet5k_socket_close(socket);
            *_errno = MP_ENOTCONN; // ??
            return -1;
        }
        mp_hal_delay_ms(1);
    }
}

STATIC int wiznet5k_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    // use "bind" function to open the socket in client mode
    if (wiznet5k_socket_bind(socket, ip, 0, _errno) != 0) {
        return -1;
    }

    // now connect
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(connect)(socket->u_param.fileno, ip, port);
    MP_THREAD_GIL_ENTER();

    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }

    // success
    return 0;
}

STATIC mp_uint_t wiznet5k_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(send)(socket->u_param.fileno, (byte*)buf, len);
    MP_THREAD_GIL_ENTER();

    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

STATIC mp_uint_t wiznet5k_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(recv)(socket->u_param.fileno, buf, len);
    MP_THREAD_GIL_ENTER();

    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

STATIC mp_uint_t wiznet5k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    if (socket->u_param.domain == 0) {
        // socket not opened; use "bind" function to open the socket in client mode
        if (wiznet5k_socket_bind(socket, ip, 0, _errno) != 0) {
            return -1;
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(sendto)(socket->u_param.fileno, (byte*)buf, len, ip, port);
    MP_THREAD_GIL_ENTER();

    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

STATIC mp_uint_t wiznet5k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    uint16_t port2;
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(recvfrom)(socket->u_param.fileno, buf, len, ip, &port2);
    MP_THREAD_GIL_ENTER();
    *port = port2;
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

STATIC int wiznet5k_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    // TODO
    *_errno = MP_EINVAL;
    return -1;
}

STATIC int wiznet5k_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    // TODO
    *_errno = MP_EINVAL;
    return -1;

    /*
    if (timeout_ms == 0) {
        // set non-blocking mode
        uint8_t arg = SOCK_IO_NONBLOCK;
        WIZCHIP_EXPORT(ctlsocket)(socket->u_param.fileno, CS_SET_IOMODE, &arg);
    }
    */
}

STATIC int wiznet5k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    if (request == MP_STREAM_POLL) {
        int ret = 0;
        if (arg & MP_STREAM_POLL_RD && getSn_RX_RSR(socket->u_param.fileno) != 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if (arg & MP_STREAM_POLL_WR && getSn_TX_FSR(socket->u_param.fileno) != 0) {
            ret |= MP_STREAM_POLL_WR;
        }
        return ret;
    } else {
        *_errno = MP_EINVAL;
        return MP_STREAM_ERROR;
    }
}

#if 0
STATIC void wiznet5k_socket_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    wiznet5k_socket_obj_t *self = self_in;
    print(env, "<WIZNET5K.socket sn=%u MR=0x%02x>", self->sn, getSn_MR(self->sn));
}

STATIC mp_obj_t wiznet5k_socket_disconnect(mp_obj_t self_in) {
    mp_int_t ret = WIZCHIP_EXPORT(disconnect)(self->sn);
    return 0;
}
#endif

/******************************************************************************/
// MicroPython bindings

/// \classmethod \constructor(spi, pin_cs, pin_rst)
/// Create and return a WIZNET5K object.
STATIC mp_obj_t wiznet5k_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 3, 3, false);

    // init the wiznet5k object
    wiznet5k_obj.base.type = (mp_obj_type_t*)&mod_network_nic_type_wiznet5k;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.spi = spi_get_handle(args[0]);
    wiznet5k_obj.cs = pin_find(args[1]);
    wiznet5k_obj.rst = pin_find(args[2]);
    wiznet5k_obj.socket_used = 0;

    /*!< SPI configuration */
    wiznet5k_obj.spi->Init.Mode = SPI_MODE_MASTER;
    wiznet5k_obj.spi->Init.Direction = SPI_DIRECTION_2LINES;
    wiznet5k_obj.spi->Init.DataSize = SPI_DATASIZE_8BIT;
    wiznet5k_obj.spi->Init.CLKPolarity = SPI_POLARITY_LOW; // clock is low when idle
    wiznet5k_obj.spi->Init.CLKPhase = SPI_PHASE_1EDGE; // data latched on first edge, which is rising edge for low-idle
    wiznet5k_obj.spi->Init.NSS = SPI_NSS_SOFT;
    wiznet5k_obj.spi->Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // clock freq = f_PCLK / this_prescale_value; Wiz820i can do up to 80MHz
    wiznet5k_obj.spi->Init.FirstBit = SPI_FIRSTBIT_MSB;
    wiznet5k_obj.spi->Init.TIMode = SPI_TIMODE_DISABLED;
    wiznet5k_obj.spi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
    wiznet5k_obj.spi->Init.CRCPolynomial = 7; // unused
    spi_init(wiznet5k_obj.spi, false);

    mp_hal_pin_output(wiznet5k_obj.cs);
    mp_hal_pin_output(wiznet5k_obj.rst);

    mp_hal_pin_low(wiznet5k_obj.rst);
    mp_hal_delay_ms(1); // datasheet says 2us
    mp_hal_pin_high(wiznet5k_obj.rst);
    mp_hal_delay_ms(160); // datasheet says 150ms

    reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
    reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
    reg_wizchip_spi_cbfunc(wiz_spi_read, wiz_spi_write);

    uint8_t sn_size[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2}; // 2k buffer for each socket
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    // set some sensible default values; they are configurable using ifconfig method
    wiz_NetInfo netinfo = {
        .mac = {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
        .ip = {192, 168, 0, 18},
        .sn = {255, 255, 255, 0},
        .gw = {192, 168, 0, 1},
        .dns = {8, 8, 8, 8}, // Google public DNS
        .dhcp = NETINFO_STATIC,
    };
    ctlnetwork(CN_SET_NETINFO, (void*)&netinfo);

    // seems we need a small delay after init
    mp_hal_delay_ms(250);

    // register with network module
    mod_network_register_nic(&wiznet5k_obj);

    // return wiznet5k object
    return &wiznet5k_obj;
}

/// \method regs()
/// Dump WIZNET5K registers.
STATIC mp_obj_t wiznet5k_regs(mp_obj_t self_in) {
    //wiznet5k_obj_t *self = self_in;
    printf("Wiz CREG:");
    for (int i = 0; i < 0x50; ++i) {
        if (i % 16 == 0) {
            printf("\n  %04x:", i);
        }
        #if MICROPY_PY_WIZNET5K == 5200
        uint32_t reg = i;
        #else
        uint32_t reg = _W5500_IO_BASE_ | i << 8;
        #endif
        printf(" %02x", WIZCHIP_READ(reg));
    }
    for (int sn = 0; sn < 4; ++sn) {
        printf("\nWiz SREG[%d]:", sn);
        for (int i = 0; i < 0x30; ++i) {
            if (i % 16 == 0) {
                printf("\n  %04x:", i);
            }
            #if MICROPY_PY_WIZNET5K == 5200
            uint32_t reg = WIZCHIP_SREG_ADDR(sn, i);
            #else
            uint32_t reg = _W5500_IO_BASE_ | i << 8 | WIZCHIP_SREG_BLOCK(sn) << 3;
            #endif
            printf(" %02x", WIZCHIP_READ(reg));
        }
    }
    printf("\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_regs_obj, wiznet5k_regs);

STATIC mp_obj_t wiznet5k_isconnected(mp_obj_t self_in) {
    (void)self_in;
    return mp_obj_new_bool(wizphy_getphylink() == PHY_LINK_ON);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(wiznet5k_isconnected_obj, wiznet5k_isconnected);

/// \method ifconfig([(ip, subnet, gateway, dns)])
/// Get/set IP address, subnet mask, gateway and DNS.
STATIC mp_obj_t wiznet5k_ifconfig(size_t n_args, const mp_obj_t *args) {
    wiz_NetInfo netinfo;
    ctlnetwork(CN_GET_NETINFO, &netinfo);
    if (n_args == 1) {
        // get
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr(netinfo.ip, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.sn, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr(netinfo.dns, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
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
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(wiznet5k_ifconfig_obj, 1, 2, wiznet5k_ifconfig);

STATIC const mp_rom_map_elem_t wiznet5k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_regs), MP_ROM_PTR(&wiznet5k_regs_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&wiznet5k_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wiznet5k_isconnected_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wiznet5k_locals_dict, wiznet5k_locals_dict_table);

const mod_network_nic_type_t mod_network_nic_type_wiznet5k = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_WIZNET5K,
        .make_new = wiznet5k_make_new,
        .locals_dict = (mp_obj_dict_t*)&wiznet5k_locals_dict,
    },
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
