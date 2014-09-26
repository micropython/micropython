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

// We can't include stdio.h because it defines _types_fd_set, but we
// need to use the CC3000 version of this type.

#include <std.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

// CC3000 defines its own ENOBUFS (different to standard one!)
// CC3000 uses global variable called errno
#undef ENOBUFS
#undef errno

#include "stm32f4xx_hal.h"
#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objtuple.h"
#include "stream.h"
#include "runtime.h"
#include "pin.h"
#include "genhdr/pins.h"
#include "spi.h"
#include "pybioctl.h"

#include "hci.h"
#include "socket.h"
#include "inet_ntop.h"
#include "inet_pton.h"
#include "ccspi.h"
#include "wlan.h"
#include "nvmem.h"
#include "netapp.h"
#include "patch_prog.h"

STATIC const mp_obj_type_t cc3k_type;
STATIC const mp_obj_type_t cc3k_socket_type;

STATIC mp_obj_t cc3k_socket_new(mp_uint_t family, mp_uint_t type, mp_uint_t protocol);

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

/******************************************************************************/
// Micro Python bindings; CC3k class

typedef struct _cc3k_obj_t {
    mp_obj_base_t base;
} cc3k_obj_t;

/// \classmethod \constructor(spi, pin_cs, pin_en, pin_irq)
/// Initialise the CC3000 using the given SPI bus and pins and return a CC3k object.
//
// Note: pins were originally hard-coded to:
//      PYBv1.0: init(pyb.SPI(2), pyb.Pin.board.Y5, pyb.Pin.board.Y4, pyb.Pin.board.Y3)
//        [SPI on Y position; Y6=B13=SCK, Y7=B14=MISO, Y8=B15=MOSI]
//
//      STM32F4DISC: init(pyb.SPI(2), pyb.Pin.cpu.A15, pyb.Pin.cpu.B10, pyb.Pin.cpu.B11)
STATIC mp_obj_t cc3k_make_new(mp_obj_t type_in, mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
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
        nlr_raise(mp_obj_new_exception_msg(
                    &mp_type_OSError, "Failed to init wlan module"));
    }

    // set connection policy. this should be called explicitly by the user
    // wlan_ioctl_set_connection_policy(0, 0, 0);

    // Mask out all non-required events from the CC3000
    wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|
                        HCI_EVNT_WLAN_UNSOL_INIT|
                        HCI_EVNT_WLAN_ASYNC_PING_REPORT|
                        HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE);

    cc3k_obj_t *cc3k = m_new_obj(cc3k_obj_t);
    cc3k->base.type = &cc3k_type;

    return cc3k;
}

STATIC mp_obj_t cc3k_connect(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    int ssid_len =0;
    const char *ssid = NULL;
    const char *bssid = NULL;

    int key_len =0;
    int sec = WLAN_SEC_UNSEC;
    const char *key = NULL;

    mp_map_elem_t *kw_key, *kw_sec, *kw_bssid;

    ssid = mp_obj_str_get_str(args[1]);
    ssid_len = strlen(ssid);

    // get KW args
    kw_key = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("key")), MP_MAP_LOOKUP);
    kw_sec = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("sec")), MP_MAP_LOOKUP);
    kw_bssid = mp_map_lookup(kw_args, MP_OBJ_NEW_QSTR(qstr_from_str("bssid")), MP_MAP_LOOKUP);

    // get key and sec
    if (kw_key && kw_sec) {
        key = mp_obj_str_get_str(kw_key->value);
        key_len = strlen(key);

        sec = mp_obj_get_int(kw_sec->value);
        if (!(WLAN_SEC_UNSEC < sec && sec <= WLAN_SEC_WPA2)) {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid security mode"));
        }
    }

    // get bssid
    if (kw_bssid != NULL) {
        bssid = mp_obj_str_get_str(kw_bssid->value);
    }

    // connect to AP
    if (wlan_connect(sec, (char*) ssid, ssid_len, (uint8_t*)bssid, (uint8_t*)key, key_len) != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "could not connect to ssid=%s, sec=%d, key=%s\n", ssid, sec, key));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3k_connect_obj, 2, cc3k_connect);

STATIC mp_obj_t cc3k_disconnect(mp_obj_t self_in) {
    int ret = wlan_disconnect();
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_disconnect_obj, cc3k_disconnect);

STATIC mp_obj_t cc3k_is_connected(mp_obj_t self_in) {
    if (wlan_connected && ip_obtained) {
        return mp_const_true;
    }
    return mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_is_connected_obj, cc3k_is_connected);

STATIC mp_obj_t cc3k_ifconfig(mp_obj_t self_in) {
    tNetappIpconfigRetArgs ipconfig={{0}};

    uint8_t *ip = &ipconfig.aucIP[0];
    uint8_t *mask= &ipconfig.aucSubnetMask[0];
    uint8_t *gw= &ipconfig.aucDefaultGateway[0];
    uint8_t *dhcp= &ipconfig.aucDHCPServer[0];
    uint8_t *dns= &ipconfig.aucDNSServer[0];
    uint8_t *mac= &ipconfig.uaMacAddr[0];
    uint8_t *ssid= &ipconfig.uaSSID[0];

    netapp_ipconfig(&ipconfig);

    printf ("IP:%d.%d.%d.%d\n"  \
            "Mask:%d.%d.%d.%d\n"\
            "GW:%d.%d.%d.%d\n"  \
            "DHCP:%d.%d.%d.%d\n"\
            "DNS:%d.%d.%d.%d\n" \
            "MAC:%02X:%02X:%02X:%02X:%02X:%02X\n"\
            "SSID: %s\n",
            ip[3], ip[2], ip[1], ip[0],
            mask[3], mask[2], mask[1], mask[0],
            gw[3], gw[2], gw[1], gw[0],
            dhcp[3], dhcp[2], dhcp[1], dhcp[0],
            dns[3], dns[2], dns[1], dns[0],
            mac[5], mac[4], mac[3], mac[2], mac[1], mac[0], ssid);
    return mp_const_none;
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

STATIC mp_obj_t cc3k_patch_program(mp_obj_t self_in) {
    //patch_prog_start();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_patch_program_obj, cc3k_patch_program);

/// \method socket(family=AF_INET, type=SOCK_STREAM, fileno=-1)
/// Create a socket.
STATIC const mp_arg_t cc3k_socket_args[] = {
    { MP_QSTR_family, MP_ARG_INT, {.u_int = AF_INET} },
    { MP_QSTR_type,   MP_ARG_INT, {.u_int = SOCK_STREAM} },
};
#define PYB_CC3K_SOCKET_NUM_ARGS MP_ARRAY_SIZE(cc3k_socket_args)
STATIC mp_obj_t cc3k_socket(mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    // parse args
    mp_arg_val_t vals[PYB_CC3K_SOCKET_NUM_ARGS];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, PYB_CC3K_SOCKET_NUM_ARGS, cc3k_socket_args, vals);

    return cc3k_socket_new(vals[0].u_int, vals[1].u_int, 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(cc3k_socket_obj, 1, cc3k_socket);

STATIC mp_obj_t cc3k_gethostbyname(mp_obj_t self_in, mp_obj_t hostname) {
    mp_uint_t len;
    const char *host = mp_obj_str_get_data(hostname, &len);
    uint32_t ip;

    if (gethostbyname((char*)host, len, &ip) < 0) {
        // TODO raise appropriate exception
        printf("gethostbyname failed\n");
        return mp_const_none;
    }

    if (ip == 0) {
        // unknown host
        // TODO CPython raises: socket.gaierror: [Errno -2] Name or service not known
        printf("Name or service not known\n");
        return mp_const_none;
    }

    // turn the ip address into a string (could use inet_ntop, but this here is much more efficient)
    VSTR_FIXED(ip_str, 16);
    vstr_printf(&ip_str, "%u.%u.%u.%u", (ip >> 24) & 0xff, (ip >> 16) & 0xff, (ip >> 8) & 0xff, ip & 0xff);
    mp_obj_t ret = mp_obj_new_str(ip_str.buf, ip_str.len, false);

    return ret;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_gethostbyname_obj, cc3k_gethostbyname);

STATIC const mp_map_elem_t cc3k_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&cc3k_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&cc3k_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_is_connected), (mp_obj_t)&cc3k_is_connected_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig), (mp_obj_t)&cc3k_ifconfig_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_patch_version), (mp_obj_t)&cc3k_patch_version_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_patch_program), (mp_obj_t)&cc3k_patch_program_obj },

    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&cc3k_socket_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gethostbyname), (mp_obj_t)&cc3k_gethostbyname_obj },

    // class constants

    { MP_OBJ_NEW_QSTR(MP_QSTR_WEP), MP_OBJ_NEW_SMALL_INT(WLAN_SEC_WEP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA), MP_OBJ_NEW_SMALL_INT(WLAN_SEC_WPA) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_WPA2), MP_OBJ_NEW_SMALL_INT(WLAN_SEC_WPA2) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET), MP_OBJ_NEW_SMALL_INT(AF_INET) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_AF_INET6), MP_OBJ_NEW_SMALL_INT(AF_INET6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_STREAM), MP_OBJ_NEW_SMALL_INT(SOCK_STREAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_DGRAM), MP_OBJ_NEW_SMALL_INT(SOCK_DGRAM) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SOCK_RAW), MP_OBJ_NEW_SMALL_INT(SOCK_RAW) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IP), MP_OBJ_NEW_SMALL_INT(IPPROTO_IP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_ICMP), MP_OBJ_NEW_SMALL_INT(IPPROTO_ICMP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IPV4), MP_OBJ_NEW_SMALL_INT(IPPROTO_IPV4) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_TCP), MP_OBJ_NEW_SMALL_INT(IPPROTO_TCP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_UDP), MP_OBJ_NEW_SMALL_INT(IPPROTO_UDP) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_IPV6), MP_OBJ_NEW_SMALL_INT(IPPROTO_IPV6) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_IPPROTO_RAW), MP_OBJ_NEW_SMALL_INT(IPPROTO_RAW) },
};

STATIC MP_DEFINE_CONST_DICT(cc3k_locals_dict, cc3k_locals_dict_table);

STATIC const mp_obj_type_t cc3k_type = {
    { &mp_type_type },
    .name = MP_QSTR_CC3k,
    //.print = cc3k_print,
    .make_new = cc3k_make_new,
    .locals_dict = (mp_obj_t)&cc3k_locals_dict,
};

/******************************************************************************/
// Micro Python bindings; CC3k socket class

#define MAX_ADDRSTRLEN      (128)
#define MAX_RX_PACKET       (CC3000_RX_BUFFER_SIZE-CC3000_MINIMAL_RX_SIZE-1)
#define MAX_TX_PACKET       (CC3000_TX_BUFFER_SIZE-CC3000_MINIMAL_TX_SIZE-1)

typedef struct _cc3k_socket_obj_t {
    mp_obj_base_t base;
    int fd;
} cc3k_socket_obj_t;

STATIC mp_obj_t cc3k_socket_new(mp_uint_t family, mp_uint_t type, mp_uint_t protocol) {
    // create socket object
    cc3k_socket_obj_t *s = m_new_obj_with_finaliser(cc3k_socket_obj_t);
    s->base.type = (mp_obj_t)&cc3k_socket_type;

    // open socket
    s->fd = socket(family, type, protocol);
    if (s->fd < 0) {
        m_del_obj(cc3k_socket_obj_t, s);
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "socket failed"));
    }

    // clear socket state
    cc3k_reset_fd_closed_state(s->fd);

    return s;
}

STATIC void cc3k_socket_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    cc3k_socket_obj_t *self = self_in;
    printf("<CC3k.socket fd=%d>", self->fd);
}

STATIC mp_uint_t cc3k_socket_send(mp_obj_t self_in, const void *buf, mp_uint_t size, int *errcode) {
    cc3k_socket_obj_t *self = self_in;

    if (cc3k_get_fd_closed_state(self->fd)) {
        closesocket(self->fd);
        *errcode = EPIPE;
        return 0;
    }

    // CC3K does not handle fragmentation, and will overflow,
    // split the packet into smaller ones and send them out.
    int bytes = 0;
    while (bytes < size) {
        int n = MIN((size-bytes), MAX_TX_PACKET);
        n = send(self->fd, buf+bytes, n, 0);
        if (n <= 0) {
            bytes = n;
            *errcode = errno;
            break;
        }
        bytes += n;
    }

    return bytes;
}

STATIC mp_uint_t cc3k_socket_recv(mp_obj_t self_in, void *buf, mp_uint_t size, int *errcode) {
    cc3k_socket_obj_t *self = self_in;

    if (cc3k_get_fd_closed_state(self->fd)) {
        closesocket(self->fd);
        return 0;
    }

    // recv MAX_RX_PACKET
    int bytes = 0;
    while (bytes < size) {
        int n = MIN((size-bytes), MAX_RX_PACKET);
        n = recv(self->fd, buf+bytes, n, 0);
        if (n == 0) {
            break;
        } else if (n < 0) {
            bytes = n;
            *errcode = errno;
            break;
        }
        bytes += n;
    }

    return bytes;
}

STATIC mp_obj_t cc3k_socket_bind(mp_obj_t self_in, mp_obj_t addr_obj) {
    cc3k_socket_obj_t *self = self_in;

    mp_obj_t *addr;
    mp_obj_get_array_fixed_n(addr_obj, 2, &addr);

    // fill sockaddr struct
    int port = mp_obj_get_int(addr[1]);
    sockaddr_in addr_in = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
        .sin_addr.s_addr = 0,// INADDR_ANY
        .sin_zero   = {0}
    };

    const char *host = mp_obj_str_get_str(addr[0]);
    if (strlen(host) && !inet_pton(AF_INET, host, &addr_in.sin_addr.s_addr)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "invalid IP address"));
    }

    // bind socket
    if (bind(self->fd, (sockaddr*) &addr_in, sizeof(sockaddr_in)) < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "bind failed"));
    }
    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_socket_bind_obj, cc3k_socket_bind);

STATIC mp_obj_t cc3k_socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
    cc3k_socket_obj_t *self = self_in;
    if (listen(self->fd, mp_obj_get_int(backlog)) < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "listen failed"));
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_socket_listen_obj, cc3k_socket_listen);

STATIC mp_obj_t cc3k_socket_accept(mp_obj_t self_in) {
    cc3k_socket_obj_t *self = self_in;
    int fd;

    sockaddr addr;
    socklen_t addr_len = sizeof(sockaddr);

    // accept incoming connection
    if ((fd = accept(self->fd, &addr, &addr_len)) < 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "accept failed"));
    }

    // clear socket state
    cc3k_reset_fd_closed_state(fd);

    // create new socket object
    cc3k_socket_obj_t *socket_obj = m_new_obj_with_finaliser(cc3k_socket_obj_t);
    socket_obj->base.type = (mp_obj_t)&cc3k_socket_type;
    socket_obj->fd  = fd;

    char buf[MAX_ADDRSTRLEN]={0};
    if (inet_ntop(addr.sa_family,
        &(((sockaddr_in*)&addr)->sin_addr), buf, MAX_ADDRSTRLEN) == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "invalid IP address"));
    }

    mp_obj_tuple_t *cli = mp_obj_new_tuple(2, NULL);
    mp_obj_tuple_t *cli_addr = mp_obj_new_tuple(2, NULL);

    cli->items[0] = socket_obj;
    cli->items[1] = cli_addr;
    cli_addr->items[0] = mp_obj_new_str(buf, strlen(buf), false);
    cli_addr->items[1] = mp_obj_new_int(((sockaddr_in*)&addr)->sin_port);

    return cli;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_socket_accept_obj, cc3k_socket_accept);

STATIC mp_obj_t cc3k_socket_connect(mp_obj_t self_in, mp_obj_t addr_obj) {
    cc3k_socket_obj_t *self = self_in;

    mp_obj_t *addr;
    mp_obj_get_array_fixed_n(addr_obj, 2, &addr);

    // fill sockaddr struct
    int port = mp_obj_get_int(addr[1]);
    sockaddr_in addr_in = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
        .sin_addr.s_addr = 0, // to be filled below using inet_pton
        .sin_zero   = {0}
    };

    const char *host = mp_obj_str_get_str(addr[0]);
    if (!inet_pton(AF_INET, host, &addr_in.sin_addr.s_addr)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "invalid IP address"));
    }

    //printf("doing connect: fd=%d, sockaddr=(%d, %d, %lu)\n", self->fd, addr_in.sin_family, addr_in.sin_port, addr_in.sin_addr.s_addr);

    int ret = connect(self->fd, (sockaddr*)&addr_in, sizeof(sockaddr_in));
    if (ret != 0) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_OSError, "[Errno %d] connect failed", ret));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_socket_connect_obj, cc3k_socket_connect);

STATIC mp_obj_t cc3k_socket_settimeout(mp_obj_t self_in, mp_obj_t timeout) {
    cc3k_socket_obj_t *self = self_in;
    int optval = mp_obj_get_int(timeout);
    socklen_t optlen = sizeof(optval);

    if (setsockopt(self->fd, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &optval, optlen) != 0) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "setsockopt failed"));
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_socket_settimeout_obj, cc3k_socket_settimeout);

STATIC mp_obj_t cc3k_socket_setblocking(mp_obj_t self_in, mp_obj_t blocking) {
    cc3k_socket_obj_t *self = self_in;
    int optval;
    socklen_t optlen = sizeof(optval);

    if (mp_obj_get_int(blocking)) {
        optval = SOCK_OFF; // Enable non-blocking
    } else {
        optval = SOCK_ON;
    }

    if (setsockopt(self->fd, SOL_SOCKET, SOCKOPT_RECV_NONBLOCK, &optval, optlen) != 0 ||
        setsockopt(self->fd, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &optval, optlen) != 0 ) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "setsockopt failed"));
    }

    return mp_const_true;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(cc3k_socket_setblocking_obj, cc3k_socket_setblocking);

STATIC mp_obj_t cc3k_socket_close(mp_obj_t self_in) {
    cc3k_socket_obj_t *self = self_in;
    closesocket(self->fd);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(cc3k_socket_close_obj, cc3k_socket_close);

STATIC const mp_map_elem_t cc3k_socket_locals_dict_table[] = {
    // TODO read/write/send/recv distinctions
    { MP_OBJ_NEW_QSTR(MP_QSTR_send),        (mp_obj_t)&mp_stream_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv),        (mp_obj_t)&mp_stream_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind),        (mp_obj_t)&cc3k_socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen),      (mp_obj_t)&cc3k_socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept),      (mp_obj_t)&cc3k_socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect),     (mp_obj_t)&cc3k_socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_settimeout),  (mp_obj_t)&cc3k_socket_settimeout_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setblocking), (mp_obj_t)&cc3k_socket_setblocking_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close),       (mp_obj_t)&cc3k_socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__),     (mp_obj_t)&cc3k_socket_close_obj },
};

STATIC MP_DEFINE_CONST_DICT(cc3k_socket_locals_dict, cc3k_socket_locals_dict_table);

mp_uint_t cc3k_ioctl(mp_obj_t self_in, mp_uint_t request, int *errcode, ...) {
    cc3k_socket_obj_t *self = self_in;
    va_list vargs;
    va_start(vargs, errcode);
    mp_uint_t ret;
    if (request == MP_IOCTL_POLL) {
        mp_uint_t flags = va_arg(vargs, mp_uint_t);
        ret = 0;
        int fd = self->fd;

        // init fds
        fd_set rfds, wfds, xfds;
        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&xfds);

        // set fds if needed
        if (flags & MP_IOCTL_POLL_RD) {
            FD_SET(fd, &rfds);

            // A socked that just closed is available for reading.  A call to
            // recv() returns 0 which is consistent with BSD.
            if (cc3k_get_fd_closed_state(fd)) {
                ret |= MP_IOCTL_POLL_RD;
            }
        }
        if (flags & MP_IOCTL_POLL_WR) {
            FD_SET(fd, &wfds);
        }
        if (flags & MP_IOCTL_POLL_HUP) {
            FD_SET(fd, &xfds);
        }

        // call cc3000 select with minimum timeout
        timeval tv;
        tv.tv_sec = 0;
        tv.tv_usec = 1;
        int nfds = select(fd + 1, &rfds, &wfds, &xfds, &tv);

        // check for error
        if (nfds == -1) {
            *errcode = errno; // return cc3000's errno
            return -1;
        }

        // check return of select
        if (FD_ISSET(fd, &rfds)) {
            ret |= MP_IOCTL_POLL_RD;
        }
        if (FD_ISSET(fd, &wfds)) {
            ret |= MP_IOCTL_POLL_WR;
        }
        if (FD_ISSET(fd, &xfds)) {
            ret |= MP_IOCTL_POLL_HUP;
        }
    } else {
        *errcode = EINVAL;
        ret = -1;
    }
    va_end(vargs);
    return ret;
}

STATIC const mp_stream_p_t cc3k_socket_stream_p = {
    .read = cc3k_socket_recv,
    .write = cc3k_socket_send,
    .ioctl = cc3k_ioctl,
    .is_text = false,
};

STATIC const mp_obj_type_t cc3k_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .print = cc3k_socket_print,
    .getiter = NULL,
    .iternext = NULL,
    .stream_p = &cc3k_socket_stream_p,
    .locals_dict = (mp_obj_t)&cc3k_socket_locals_dict,
};

/******************************************************************************/
// Micro Python bindings; CC3k module

STATIC const mp_map_elem_t mp_module_cc3k_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cc3k) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_CC3k), (mp_obj_t)&cc3k_type },
};

STATIC const mp_obj_dict_t mp_module_cc3k_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = MP_ARRAY_SIZE(mp_module_cc3k_globals_table),
        .alloc = MP_ARRAY_SIZE(mp_module_cc3k_globals_table),
        .table = (mp_map_elem_t*)mp_module_cc3k_globals_table,
    },
};

const mp_obj_module_t mp_module_cc3k = {
    .base = { &mp_type_module },
    .name = MP_QSTR_cc3k,
    .globals = (mp_obj_dict_t*)&mp_module_cc3k_globals,
};
