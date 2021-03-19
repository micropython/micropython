/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2014 Damien P. George
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
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/stream.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"

#if MICROPY_PY_WIZNET5K

#include "shared-module/network/__init__.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/busio/SPI.h"

#include "shared-module/network/__init__.h"

#include "ethernet/wizchip_conf.h"
#include "ethernet/socket.h"
#include "internet/dns/dns.h"
#include "internet/dhcp/dhcp.h"

#include "shared-module/wiznet/wiznet5k.h"

STATIC wiznet5k_obj_t wiznet5k_obj;

STATIC void wiz_cris_enter(void) {
    wiznet5k_obj.cris_state = MICROPY_BEGIN_ATOMIC_SECTION();
}

STATIC void wiz_cris_exit(void) {
    MICROPY_END_ATOMIC_SECTION(wiznet5k_obj.cris_state);
}

STATIC void wiz_cs_select(void) {
    common_hal_digitalio_digitalinout_set_value(&wiznet5k_obj.cs, 0);
}

STATIC void wiz_cs_deselect(void) {
    common_hal_digitalio_digitalinout_set_value(&wiznet5k_obj.cs, 1);
}

STATIC void wiz_spi_read(uint8_t *buf, uint32_t len) {
    (void)common_hal_busio_spi_read(wiznet5k_obj.spi, buf, len, 0);
}

STATIC void wiz_spi_write(const uint8_t *buf, uint32_t len) {
    (void)common_hal_busio_spi_write(wiznet5k_obj.spi, buf, len);
}

int wiznet5k_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    uint8_t dns_ip[MOD_NETWORK_IPADDR_BUF_SIZE] = {8, 8, 8, 8};
    uint8_t *buf = m_new(uint8_t, MAX_DNS_BUF_SIZE);
    DNS_init(0, buf);
    mp_int_t ret = DNS_run(dns_ip, (uint8_t *)name, out_ip);
    m_del(uint8_t, buf, MAX_DNS_BUF_SIZE);
    if (ret == 1) {
        // success
        return 0;
    } else {
        // failure
        return -2;
    }
}

int get_available_socket(wiznet5k_obj_t *wiz) {
    for (uint8_t sn = 0; sn < _WIZCHIP_SOCK_NUM_; sn++) {
        if ((wiz->socket_used & (1 << sn)) == 0) {
            wiz->socket_used |= (1 << sn);
            return sn;
        }
    }
    return -1;
}

int wiznet5k_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->u_param.domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }

    switch (socket->u_param.type) {
        case MOD_NETWORK_SOCK_STREAM:
            socket->u_param.type = Sn_MR_TCP;
            break;
        case MOD_NETWORK_SOCK_DGRAM:
            socket->u_param.type = Sn_MR_UDP;
            break;
        default:
            *_errno = MP_EINVAL;
            return -1;
    }

    if (socket->u_param.fileno == -1) {
        // get first unused socket number
        socket->u_param.fileno = get_available_socket(&wiznet5k_obj);
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

void wiznet5k_socket_close(mod_network_socket_obj_t *socket) {
    uint8_t sn = (uint8_t)socket->u_param.fileno;
    if (sn < _WIZCHIP_SOCK_NUM_) {
        wiznet5k_obj.socket_used &= ~(1 << sn);
        WIZCHIP_EXPORT(close)(sn);
    }
}

int wiznet5k_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
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

int wiznet5k_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    mp_int_t ret = WIZCHIP_EXPORT(listen)(socket->u_param.fileno);
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return 0;
}

int wiznet5k_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
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
                // printf("(bad resocket %d)\n", _errno2);
            } else if (wiznet5k_socket_bind(socket, NULL, *port, &_errno2) != 0) {
                // printf("(bad rebind %d)\n", _errno2);
            } else if (wiznet5k_socket_listen(socket, 0, &_errno2) != 0) {
                // printf("(bad relisten %d)\n", _errno2);
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

int wiznet5k_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    uint16_t src_port = network_module_create_random_source_tcp_port();
    // make sure same outgoing port number can't be in use by two different sockets.
    src_port = (src_port & ~(_WIZCHIP_SOCK_NUM_ - 1)) | socket->u_param.fileno;

    // use "bind" function to open the socket in client mode
    if (wiznet5k_socket_bind(socket, NULL, src_port, _errno) != 0) {
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

mp_uint_t wiznet5k_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(send)(socket->u_param.fileno, (byte *)buf, len);
    MP_THREAD_GIL_ENTER();

    // TODO convert Wiz errno's to POSIX ones
    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

mp_uint_t wiznet5k_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
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

mp_uint_t wiznet5k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    if (socket->u_param.domain == 0) {
        // socket not opened; use "bind" function to open the socket in client mode
        if (wiznet5k_socket_bind(socket, ip, 0, _errno) != 0) {
            return -1;
        }
    }

    MP_THREAD_GIL_EXIT();
    mp_int_t ret = WIZCHIP_EXPORT(sendto)(socket->u_param.fileno, (byte *)buf, len, ip, port);
    MP_THREAD_GIL_ENTER();

    if (ret < 0) {
        wiznet5k_socket_close(socket);
        *_errno = -ret;
        return -1;
    }
    return ret;
}

mp_uint_t wiznet5k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
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

int wiznet5k_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    // TODO
    *_errno = MP_EINVAL;
    return -1;
}

int wiznet5k_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
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

int wiznet5k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
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

void wiznet5k_socket_timer_tick(mod_network_socket_obj_t *socket) {
    if (wiznet5k_obj.dhcp_socket >= 0) {
        DHCP_time_handler();
        DHCP_run();
    }
}

int wiznet5k_start_dhcp(void) {
    // XXX this should throw an error if DHCP fails
    static DHCP_INIT_BUFFER_TYPE dhcp_buf[DHCP_INIT_BUFFER_SIZE];

    if (wiznet5k_obj.dhcp_socket < 0) {
        // Set up the socket to listen on UDP 68 before calling DHCP_init
        wiznet5k_obj.dhcp_socket = get_available_socket(&wiznet5k_obj);
        if (wiznet5k_obj.dhcp_socket < 0) {
            return MP_EMFILE;
        }

        WIZCHIP_EXPORT(socket)(wiznet5k_obj.dhcp_socket, MOD_NETWORK_SOCK_DGRAM, DHCP_CLIENT_PORT, 0);
        DHCP_init(wiznet5k_obj.dhcp_socket, dhcp_buf);
    }
    return 0;
}

int wiznet5k_stop_dhcp(void) {
    if (wiznet5k_obj.dhcp_socket >= 0) {
        DHCP_stop();
        WIZCHIP_EXPORT(close)(wiznet5k_obj.dhcp_socket);
        wiznet5k_obj.socket_used &= ~(1 << wiznet5k_obj.dhcp_socket);
        wiznet5k_obj.dhcp_socket = -1;
    }
    return 0;
}

bool wiznet5k_check_dhcp(void) {
    return wiznet5k_obj.dhcp_socket >= 0;
}

void wiznet5k_reset(void) {
    if (wiznet5k_obj.rst.pin) {
        // hardware reset if using RST pin
        common_hal_digitalio_digitalinout_set_value(&wiznet5k_obj.rst, 0);
        mp_hal_delay_us(10); // datasheet says 2us
        common_hal_digitalio_digitalinout_set_value(&wiznet5k_obj.rst, 1);
        mp_hal_delay_ms(150); // datasheet says 150ms
    } else {
        // otherwise, software reset
        wizchip_sw_reset();
    }
}

void wiznet5k_socket_deinit(mod_network_socket_obj_t *socket) {
    wiznet5k_reset();
}

/// Create and return a WIZNET5K object.
mp_obj_t wiznet5k_create(busio_spi_obj_t *spi_in, const mcu_pin_obj_t *cs_in, const mcu_pin_obj_t *rst_in) {

    // init the wiznet5k object
    wiznet5k_obj.base.type = (mp_obj_type_t *)&mod_network_nic_type_wiznet5k;
    wiznet5k_obj.cris_state = 0;
    wiznet5k_obj.spi = spi_in;
    wiznet5k_obj.socket_used = 0;
    wiznet5k_obj.dhcp_socket = -1;

    /*!< SPI configuration */
    // XXX probably should check if the provided SPI is already configured, and
    // if so skip configuration?

    common_hal_busio_spi_configure(wiznet5k_obj.spi,
        10000000,  // BAUDRATE 10MHz
        1, // HIGH POLARITY
        1, // SECOND PHASE TRANSITION
        8 // 8 BITS
        );

    common_hal_digitalio_digitalinout_construct(&wiznet5k_obj.cs, cs_in);
    common_hal_digitalio_digitalinout_switch_to_output(&wiznet5k_obj.cs, 1, DRIVE_MODE_PUSH_PULL);

    if (rst_in) {
        common_hal_digitalio_digitalinout_construct(&wiznet5k_obj.rst, rst_in);
    }
    wiznet5k_reset();

    reg_wizchip_cris_cbfunc(wiz_cris_enter, wiz_cris_exit);
    reg_wizchip_cs_cbfunc(wiz_cs_select, wiz_cs_deselect);
    reg_wizchip_spi_cbfunc(wiz_spi_read, wiz_spi_write);

    // 2k buffer for each socket
    uint8_t sn_size[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    ctlwizchip(CW_INIT_WIZCHIP, sn_size);

    wiz_NetInfo netinfo = {
        .dhcp = NETINFO_DHCP,
    };
    network_module_create_random_mac_address(netinfo.mac);
    ctlnetwork(CN_SET_NETINFO, (void *)&netinfo);

    // seems we need a small delay after init
    mp_hal_delay_ms(250);

    // register with network module
    network_module_register_nic(&wiznet5k_obj);

    // return wiznet5k object
    return &wiznet5k_obj;
}

#endif // MICROPY_PY_WIZNET5K
