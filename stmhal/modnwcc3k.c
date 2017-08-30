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

#include <string.h>
#include <stdarg.h>

// CC3000 defines its own ENOBUFS (different to standard one!)
#undef ENOBUFS

#include "py/nlr.h"
#include "py/objtuple.h"
#include "py/objlist.h"
#include "py/stream.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "lib/netutils/netutils.h"
#include "modnetwork.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"

#include "hci.h"
#include "socket.h"
#include "inet_ntop.h"
#include "inet_pton.h"
#include "ccspi.h"
#include "wlan.h"
#include "nvmem.h"
#include "netapp.h"
#include "patch_prog.h"

#define MAX_ADDRSTRLEN      (128)
#define MAX_RX_PACKET       (CC3000_RX_BUFFER_SIZE-CC3000_MINIMAL_RX_SIZE-1)
#define MAX_TX_PACKET       (CC3000_TX_BUFFER_SIZE-CC3000_MINIMAL_TX_SIZE-1)

#define MAKE_SOCKADDR(addr, ip, port) \
    sockaddr addr; \
    addr.sa_family = AF_INET; \
    addr.sa_data[0] = port >> 8; \
    addr.sa_data[1] = port; \
    addr.sa_data[2] = ip[0]; \
    addr.sa_data[3] = ip[1]; \
    addr.sa_data[4] = ip[2]; \
    addr.sa_data[5] = ip[3];

#define UNPACK_SOCKADDR(addr, ip, port) \
    port = (addr.sa_data[0] << 8) | addr.sa_data[1]; \
    ip[0] = addr.sa_data[2]; \
    ip[1] = addr.sa_data[3]; \
    ip[2] = addr.sa_data[4]; \
    ip[3] = addr.sa_data[5];

STATIC int cc3k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno);

int CC3000_EXPORT(errno); // for cc3000 driver

STATIC volatile uint32_t fd_closed_state = 0;
STATIC volatile bool wlan_connected = false;
STATIC volatile bool ip_obtained = false;

STATIC int cc3k_get_fd_closed_state(int fd) {
    return fd_closed_state & (1 << fd);
}

STATIC void cc3k_set_fd_closed_state(int fd) {
    fd_closed_state |= 1 << fd;
}

STATIC void cc3k_reset_fd_closed_state(int fd) {
    fd_closed_state &= ~(1 << fd);
}

STATIC void cc3k_callback(long event_type, char *data, unsigned char length) {
    switch (event_type) {
        case HCI_EVNT_WLAN_UNSOL_CONNECT:
            wlan_connected = true;
            break;
        case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
            // link down
            wlan_connected = false;
            ip_obtained = false;
            break;
        case HCI_EVNT_WLAN_UNSOL_DHCP:
            ip_obtained = true;
            break;
        case HCI_EVNT_BSD_TCP_CLOSE_WAIT:
            // mark socket for closure
            cc3k_set_fd_closed_state(data[0]);
            break;
    }
}

STATIC int cc3k_gethostbyname(mp_obj_t nic, const char *name, mp_uint_t len, uint8_t *out_ip) {
    uint32_t ip;
    // CC3000 gethostbyname is unreliable and usually returns -95 on first call
    for (int retry = 5; CC3000_EXPORT(gethostbyname)((char*)name, len, &ip) < 0; retry--) {
        if (retry == 0 || CC3000_EXPORT(errno) != -95) {
            return CC3000_EXPORT(errno);
        }
        mp_hal_delay_ms(50);
    }

    if (ip == 0) {
        // unknown host
        return MP_ENOENT;
    }

    out_ip[0] = ip >> 24;
    out_ip[1] = ip >> 16;
    out_ip[2] = ip >> 8;
    out_ip[3] = ip;

    return 0;
}

STATIC int cc3k_socket_socket(mod_network_socket_obj_t *socket, int *_errno) {
    if (socket->u_param.domain != MOD_NETWORK_AF_INET) {
        *_errno = MP_EAFNOSUPPORT;
        return -1;
    }

    mp_uint_t type;
    switch (socket->u_param.type) {
        case MOD_NETWORK_SOCK_STREAM: type = SOCK_STREAM; break;
        case MOD_NETWORK_SOCK_DGRAM: type = SOCK_DGRAM; break;
        case MOD_NETWORK_SOCK_RAW: type = SOCK_RAW; break;
        default: *_errno = MP_EINVAL; return -1;
    }

    // open socket
    int fd = CC3000_EXPORT(socket)(AF_INET, type, 0);
    if (fd < 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }

    // clear socket state
    cc3k_reset_fd_closed_state(fd);

    // store state of this socket
    socket->u_state = fd;

    // make accept blocking by default
    int optval = SOCK_OFF;
    socklen_t optlen = sizeof(optval);
    CC3000_EXPORT(setsockopt)(socket->u_state, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &optval, optlen);

    return 0;
}

STATIC void cc3k_socket_close(mod_network_socket_obj_t *socket) {
    CC3000_EXPORT(closesocket)(socket->u_state);
}

STATIC int cc3k_socket_bind(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = CC3000_EXPORT(bind)(socket->u_state, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int cc3k_socket_listen(mod_network_socket_obj_t *socket, mp_int_t backlog, int *_errno) {
    int ret = CC3000_EXPORT(listen)(socket->u_state, backlog);
    if (ret != 0) {
        *_errno = ret;
        return -1;
    }
    return 0;
}

STATIC int cc3k_socket_accept(mod_network_socket_obj_t *socket, mod_network_socket_obj_t *socket2, byte *ip, mp_uint_t *port, int *_errno) {
    // accept incoming connection
    int fd;
    sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    if ((fd = CC3000_EXPORT(accept)(socket->u_state, &addr, &addr_len)) < 0) {
        if (fd == SOC_IN_PROGRESS) {
            *_errno = MP_EAGAIN;
        } else {
            *_errno = -fd;
        }
        return -1;
    }

    // clear socket state
    cc3k_reset_fd_closed_state(fd);

    // store state in new socket object
    socket2->u_state = fd;

    // return ip and port
    // it seems CC3000 returns little endian for accept??
    //UNPACK_SOCKADDR(addr, ip, *port);
    *port = (addr.sa_data[1] << 8) | addr.sa_data[0];
    ip[3] = addr.sa_data[2];
    ip[2] = addr.sa_data[3];
    ip[1] = addr.sa_data[4];
    ip[0] = addr.sa_data[5];

    return 0;
}

STATIC int cc3k_socket_connect(mod_network_socket_obj_t *socket, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = CC3000_EXPORT(connect)(socket->u_state, &addr, sizeof(addr));
    if (ret != 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }
    return 0;
}

STATIC mp_uint_t cc3k_socket_send(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, int *_errno) {
    if (cc3k_get_fd_closed_state(socket->u_state)) {
        CC3000_EXPORT(closesocket)(socket->u_state);
        *_errno = MP_EPIPE;
        return -1;
    }

    // CC3K does not handle fragmentation, and will overflow,
    // split the packet into smaller ones and send them out.
    mp_int_t bytes = 0;
    while (bytes < len) {
        int n = MIN((len - bytes), MAX_TX_PACKET);
        n = CC3000_EXPORT(send)(socket->u_state, (uint8_t*)buf + bytes, n, 0);
        if (n <= 0) {
            *_errno = CC3000_EXPORT(errno);
            return -1;
        }
        bytes += n;
    }

    return bytes;
}

STATIC mp_uint_t cc3k_socket_recv(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, int *_errno) {
    // check the socket is open
    if (cc3k_get_fd_closed_state(socket->u_state)) {
        // socket is closed, but CC3000 may have some data remaining in buffer, so check
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(socket->u_state, &rfds);
        cc3000_timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int nfds = CC3000_EXPORT(select)(socket->u_state + 1, &rfds, NULL, NULL, &tv);
        if (nfds == -1 || !FD_ISSET(socket->u_state, &rfds)) {
            // no data waiting, so close socket and return 0 data
            CC3000_EXPORT(closesocket)(socket->u_state);
            return 0;
        }
    }

    // cap length at MAX_RX_PACKET
    len = MIN(len, MAX_RX_PACKET);

    // do the recv
    int ret = CC3000_EXPORT(recv)(socket->u_state, buf, len, 0);
    if (ret < 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }

    return ret;
}

STATIC mp_uint_t cc3k_socket_sendto(mod_network_socket_obj_t *socket, const byte *buf, mp_uint_t len, byte *ip, mp_uint_t port, int *_errno) {
    MAKE_SOCKADDR(addr, ip, port)
    int ret = CC3000_EXPORT(sendto)(socket->u_state, (byte*)buf, len, 0, (sockaddr*)&addr, sizeof(addr));
    if (ret < 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }
    return ret;
}

STATIC mp_uint_t cc3k_socket_recvfrom(mod_network_socket_obj_t *socket, byte *buf, mp_uint_t len, byte *ip, mp_uint_t *port, int *_errno) {
    sockaddr addr;
    socklen_t addr_len = sizeof(addr);
    mp_int_t ret = CC3000_EXPORT(recvfrom)(socket->u_state, buf, len, 0, &addr, &addr_len);
    if (ret < 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }
    UNPACK_SOCKADDR(addr, ip, *port);
    return ret;
}

STATIC int cc3k_socket_setsockopt(mod_network_socket_obj_t *socket, mp_uint_t level, mp_uint_t opt, const void *optval, mp_uint_t optlen, int *_errno) {
    int ret = CC3000_EXPORT(setsockopt)(socket->u_state, level, opt, optval, optlen);
    if (ret < 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }
    return 0;
}

STATIC int cc3k_socket_settimeout(mod_network_socket_obj_t *socket, mp_uint_t timeout_ms, int *_errno) {
    int ret;
    if (timeout_ms == 0 || timeout_ms == -1) {
        int optval;
        socklen_t optlen = sizeof(optval);
        if (timeout_ms == 0) {
            // set non-blocking mode
            optval = SOCK_ON;
        } else {
            // set blocking mode
            optval = SOCK_OFF;
        }
        ret = CC3000_EXPORT(setsockopt)(socket->u_state, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK, &optval, optlen);
        if (ret == 0) {
            ret = CC3000_EXPORT(setsockopt)(socket->u_state, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &optval, optlen);
        }
    } else {
        // set timeout
        socklen_t optlen = sizeof(timeout_ms);
        ret = CC3000_EXPORT(setsockopt)(socket->u_state, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout_ms, optlen);
    }

    if (ret != 0) {
        *_errno = CC3000_EXPORT(errno);
        return -1;
    }

    return 0;
}

STATIC int cc3k_socket_ioctl(mod_network_socket_obj_t *socket, mp_uint_t request, mp_uint_t arg, int *_errno) {
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        int fd = socket->u_state;

        // init fds
        fd_set rfds, wfds, xfds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&xfds);

        // set fds if needed
        if (flags & MP_STREAM_POLL_RD) {
            FD_SET(fd, &rfds);

            // A socked that just closed is available for reading.  A call to
            // recv() returns 0 which is consistent with BSD.
            if (cc3k_get_fd_closed_state(fd)) {
                ret |= MP_STREAM_POLL_RD;
            }
        }
        if (flags & MP_STREAM_POLL_WR) {
            FD_SET(fd, &wfds);
        }
        if (flags & MP_STREAM_POLL_HUP) {
            FD_SET(fd, &xfds);
        }

        // call cc3000 select with minimum timeout
        cc3000_timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int nfds = CC3000_EXPORT(select)(fd + 1, &rfds, &wfds, &xfds, &tv);

        // check for error
        if (nfds == -1) {
            *_errno = CC3000_EXPORT(errno);
            return -1;
        }

        // check return of select
        if (FD_ISSET(fd, &rfds)) {
            ret |= MP_STREAM_POLL_RD;
        }
        if (FD_ISSET(fd, &wfds)) {
            ret |= MP_STREAM_POLL_WR;
        }
        if (FD_ISSET(fd, &xfds)) {
            ret |= MP_STREAM_POLL_HUP;
        }
    } else {
        *_errno = MP_EINVAL;
        ret = -1;
    }
    return ret;
}

/******************************************************************************/
// MicroPython bindings; CC3K class

typedef struct _cc3k_obj_t {
    mp_obj_base_t base;
} cc3k_obj_t;

STATIC const cc3k_obj_t cc3k_obj = {{(mp_obj_type_t*)&mod_network_nic_type_cc3k}};

// \classmethod \constructor(spi, pin_cs, pin_en, pin_irq)
// Initialise the CC3000 using the given SPI bus and pins and return a CC3K object.
//
// Note: pins were originally hard-coded to:
//      PYBv1.0: init(pyb.SPI(2), pyb.Pin.board.Y5, pyb.Pin.board.Y4, pyb.Pin.board.Y3)
//        [SPI on Y position; Y6=B13=SCK, Y7=B14=MISO, Y8=B15=MOSI]
//
//      STM32F4DISC: init(pyb.SPI(2), pyb.Pin.cpu.A15, pyb.Pin.cpu.B10, pyb.Pin.cpu.B11)
STATIC mp_obj_t cc3k_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 4, 4, false);

    // set the pins to use
    SpiInit(
        spi_get_handle(args[0]),
        pin_find(args[1]),
        pin_find(args[2]),
        pin_find(args[3])
    );

    // initialize and start the module
    wlan_init(cc3k_callback, NULL, NULL, NULL,
            ReadWlanInterruptPin, SpiResumeSpi, SpiPauseSpi, WriteWlanPin);

    if (wlan_start(0) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "failed to init CC3000 module"));
    }

    // set connection policy. this should be called explicitly by the user
    // wlan_ioctl_set_connection_policy(0, 0, 0);

    // Mask out all non-required events from the CC3000
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|
                        HCI_EVNT_WLAN_UNSOL_INIT|
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT|
                        HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE);

    // register with network module
    mod_network_register_nic((mp_obj_t)&cc3k_obj);

    return (mp_obj_t)&cc3k_obj;
}

// method connect(ssid, key=None, *, security=WPA2, bssid=None)
STATIC mp_obj_t cc3k_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_ssid, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_key, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = WLAN_SEC_WPA2} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    // get ssid
    size_t ssid_len;
    const char *ssid = mp_obj_str_get_data(args[0].u_obj, &ssid_len);

    // get key and sec
    size_t key_len = 0;
    const char *key = NULL;
    mp_uint_t sec = WLAN_SEC_UNSEC;
    if (args[1].u_obj != mp_const_none) {
        key = mp_obj_str_get_data(args[1].u_obj, &key_len);
        sec = args[2].u_int;
    }

    // get bssid
    const char *bssid = NULL;
    if (args[3].u_obj != mp_const_none) {
        bssid = mp_obj_str_get_str(args[3].u_obj);
    }

    // connect to AP
    if (wlan_connect(sec, (char*)ssid, ssid_len, (uint8_t*)bssid, (uint8_t*)key, key_len) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "could not connect to ssid=%s, sec=%d, key=%s\n", ssid, sec, key));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3k_connect_obj, 1, cc3k_connect);

STATIC mp_obj_t cc3k_disconnect(mp_obj_t self_in) {
    // should we check return value?
    wlan_disconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_disconnect_obj, cc3k_disconnect);

STATIC mp_obj_t cc3k_isconnected(mp_obj_t self_in) {
    return mp_obj_new_bool(wlan_connected && ip_obtained);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_isconnected_obj, cc3k_isconnected);

STATIC mp_obj_t cc3k_ifconfig(mp_obj_t self_in) {
    tNetappIpconfigRetArgs ipconfig;
    netapp_ipconfig(&ipconfig);

    // render MAC address
    VSTR_FIXED(mac_vstr, 18);
    const uint8_t *mac = ipconfig.uaMacAddr;
    vstr_printf(&mac_vstr, "%02x:%02x:%02x:%02x:%02x:%02x", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);

    // create and return tuple with ifconfig info
    mp_obj_t tuple[7] = {
        netutils_format_ipv4_addr(ipconfig.aucIP, NETUTILS_LITTLE),
        netutils_format_ipv4_addr(ipconfig.aucSubnetMask, NETUTILS_LITTLE),
        netutils_format_ipv4_addr(ipconfig.aucDefaultGateway, NETUTILS_LITTLE),
        netutils_format_ipv4_addr(ipconfig.aucDNSServer, NETUTILS_LITTLE),
        netutils_format_ipv4_addr(ipconfig.aucDHCPServer, NETUTILS_LITTLE),
        mp_obj_new_str(mac_vstr.buf, mac_vstr.len, false),
        mp_obj_new_str((const char*)ipconfig.uaSSID, strlen((const char*)ipconfig.uaSSID), false),
    };
    return mp_obj_new_tuple(MP_ARRAY_SIZE(tuple), tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_ifconfig_obj, cc3k_ifconfig);

STATIC mp_obj_t cc3k_patch_version(mp_obj_t self_in) {
    uint8_t pver[2];
    mp_obj_tuple_t *t_pver;

    nvmem_read_sp_version(pver);
    t_pver = mp_obj_new_tuple(2, NULL);
    t_pver->items[0] = mp_obj_new_int(pver[0]);
    t_pver->items[1] = mp_obj_new_int(pver[1]);
    return t_pver;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_patch_version_obj, cc3k_patch_version);

STATIC mp_obj_t cc3k_patch_program(mp_obj_t self_in, mp_obj_t key_in) {
    const char *key = mp_obj_str_get_str(key_in);
    if (key[0] == 'p' && key[1] == 'g' && key[2] == 'm' && key[3] == '\0') {
        patch_prog_start();
    } else {
        mp_print_str(&mp_plat_print, "pass 'pgm' as argument in order to program\n");
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_patch_program_obj, cc3k_patch_program);

STATIC const mp_rom_map_elem_t cc3k_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&cc3k_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&cc3k_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&cc3k_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&cc3k_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_patch_version), MP_ROM_PTR(&cc3k_patch_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_patch_program), MP_ROM_PTR(&cc3k_patch_program_obj) },

    // class constants
    { MP_ROM_QSTR(MP_QSTR_WEP), MP_ROM_INT(WLAN_SEC_WEP) },
    { MP_ROM_QSTR(MP_QSTR_WPA), MP_ROM_INT(WLAN_SEC_WPA) },
    { MP_ROM_QSTR(MP_QSTR_WPA2), MP_ROM_INT(WLAN_SEC_WPA2) },
};

STATIC MP_DEFINE_CONST_DICT(cc3k_locals_dict, cc3k_locals_dict_table);

const mod_network_nic_type_t mod_network_nic_type_cc3k = {
    .base = {
        { &mp_type_type },
        .name = MP_QSTR_CC3K,
        .make_new = cc3k_make_new,
        .locals_dict = (mp_obj_dict_t*)&cc3k_locals_dict,
    },
    .gethostbyname = cc3k_gethostbyname,
    .socket = cc3k_socket_socket,
    .close = cc3k_socket_close,
    .bind = cc3k_socket_bind,
    .listen = cc3k_socket_listen,
    .accept = cc3k_socket_accept,
    .connect = cc3k_socket_connect,
    .send = cc3k_socket_send,
    .recv = cc3k_socket_recv,
    .sendto = cc3k_socket_sendto,
    .recvfrom = cc3k_socket_recvfrom,
    .setsockopt = cc3k_socket_setsockopt,
    .settimeout = cc3k_socket_settimeout,
    .ioctl = cc3k_socket_ioctl,
};
