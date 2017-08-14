/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Paul Sokolovsky
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
#include <stdbool.h>

#include "py/nlr.h"
#include "py/obj.h"
#include "py/gc.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "drivers/dht/dht.h"
#include "netutils.h"
#include "queue.h"
#include "ets_sys.h"
#include "uart.h"
#include "user_interface.h"
#include "espconn.h"
#include "spi_flash.h"
#include "mem.h"
#include "espneopixel.h"
#include "espapa102.h"
#include "modpyb.h"
#include "modpybrtc.h"

#define MODESP_ESPCONN (0)

#if MODESP_ESPCONN
STATIC const mp_obj_type_t esp_socket_type;

typedef struct _esp_socket_obj_t {
    mp_obj_base_t base;
    struct espconn *espconn;

    mp_obj_t cb_connect;
    mp_obj_t cb_recv;
    mp_obj_t cb_sent;
    mp_obj_t cb_disconnect;

    uint8_t *recvbuf;
    mp_uint_t recvbuf_len;

    bool fromserver;

    mp_obj_list_t *connlist;
} esp_socket_obj_t;

// Due to the onconnect callback not being able to recognize the parent esp_socket,
// we can have only one esp_socket listening at a time
// This should be solvable by some PIC hacking
STATIC esp_socket_obj_t *esp_socket_listening;

STATIC mp_obj_t esp_socket_make_new_base() {
    esp_socket_obj_t *s = m_new_obj_with_finaliser(esp_socket_obj_t);
    s->recvbuf = NULL;
    s->base.type = (mp_obj_t)&esp_socket_type;
    s->cb_connect = mp_const_none;
    s->cb_recv = mp_const_none;
    s->cb_disconnect = mp_const_none;
    s->cb_sent = mp_const_none;
    s->fromserver = false;
    s->connlist = NULL;
    return s;
}

// constructor esp_socket(family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, fileno=None)
// Arguments ignored as we do not support UDP (yet)
STATIC mp_obj_t esp_socket_make_new(const mp_obj_type_t *type_in, mp_uint_t n_args,
    mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    esp_socket_obj_t *s = esp_socket_make_new_base();
    s->espconn = m_new_obj(struct espconn);

    s->espconn->reverse = s;
    // TODO: UDP Support
    s->espconn->type = ESPCONN_TCP;
    s->espconn->state = ESPCONN_NONE;

    s->espconn->proto.tcp = m_new_obj(esp_tcp);

    return s;
}

// method socket.close()
STATIC mp_obj_t esp_socket_close(mp_obj_t self_in) {
    esp_socket_obj_t *s = self_in;

    if (esp_socket_listening == s) {
        esp_socket_listening = NULL;
    }

    if (s->espconn->state != ESPCONN_NONE && s->espconn->state != ESPCONN_CLOSE) {
        espconn_disconnect(s->espconn);
    }

    if (s->connlist != NULL) {
        mp_obj_list_set_len(s->connlist, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_socket_close_obj, esp_socket_close);

// method socket.__del__()
STATIC mp_obj_t esp_socket___del__(mp_obj_t self_in) {
    esp_socket_obj_t *s = self_in;

    esp_socket_close(self_in);

    if (s->fromserver) {
        espconn_delete(s->espconn);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_socket___del___obj, esp_socket___del__);

// method socket.bind(address)
STATIC mp_obj_t esp_socket_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    esp_socket_obj_t *s = self_in;

    mp_uint_t port = netutils_parse_inet_addr(addr_in,
        s->espconn->proto.tcp->remote_ip, NETUTILS_BIG);
    s->espconn->proto.tcp->local_port = port;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_bind_obj, esp_socket_bind);

STATIC void esp_socket_recv_callback(void *arg, char *pdata, unsigned short len) {
    struct espconn *conn = arg;
    esp_socket_obj_t *s = conn->reverse;

    if (s->cb_recv != mp_const_none) {
        call_function_2_protected(s->cb_recv, s, mp_obj_new_bytes((byte *)pdata, len));
    } else {
        if (s->recvbuf == NULL) {
            s->recvbuf = m_new(uint8_t, len);
            s->recvbuf_len = len;
            if (s->recvbuf != NULL) {
                memcpy(s->recvbuf, pdata, len);
            }
        } else {
            s->recvbuf = m_renew(uint8_t, s->recvbuf, s->recvbuf_len, s->recvbuf_len + len);
            if (s->recvbuf != NULL) {
                memcpy(&s->recvbuf[s->recvbuf_len], pdata, len);
                s->recvbuf_len += len;
            }
        }
        if (s->recvbuf == NULL) {
            esp_socket_close(s);
            return;
        }
    }
}

STATIC void esp_socket_sent_callback(void *arg) {
    struct espconn *conn = arg;
    esp_socket_obj_t *s = conn->reverse;

    if (s->cb_sent != mp_const_none) {
        call_function_1_protected(s->cb_sent, s);
    }
}

STATIC void esp_socket_disconnect_callback(void *arg) {
    struct espconn *conn = arg;
    esp_socket_obj_t *s = conn->reverse;

    if (s->cb_disconnect != mp_const_none) {
        call_function_1_protected(s->cb_disconnect, s);
    }

    esp_socket_close(s);
}

STATIC void esp_socket_connect_callback_server(void *arg) {
    struct espconn *conn = arg;

    esp_socket_obj_t *s = esp_socket_make_new_base();
    s->espconn = conn;
    s->fromserver = true;
    conn->reverse = s;

    espconn_regist_recvcb(conn, esp_socket_recv_callback);
    espconn_regist_sentcb(conn, esp_socket_sent_callback);
    espconn_regist_disconcb(conn, esp_socket_disconnect_callback);
    espconn_regist_time(conn, 15, 0);

    if (esp_socket_listening->cb_connect != mp_const_none) {
        call_function_1_protected(esp_socket_listening->cb_connect, s);
    } else {
        mp_obj_list_append(esp_socket_listening->connlist, s);
    }
}

STATIC void esp_socket_connect_callback_client(void *arg) {
    struct espconn *conn = arg;
    esp_socket_obj_t *s = conn->reverse;

    if (s->cb_connect != mp_const_none) {
        call_function_1_protected(s->cb_connect, s);
    }
}

// method socket.listen(backlog)
STATIC mp_obj_t esp_socket_listen(mp_obj_t self_in, mp_obj_t backlog) {
    esp_socket_obj_t *s = self_in;

    if (esp_socket_listening != NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "only one espconn can listen at a time"));
    }

    esp_socket_listening = s;

    s->connlist = mp_obj_new_list(0, NULL);

    espconn_regist_connectcb(s->espconn, esp_socket_connect_callback_server);
    espconn_accept(s->espconn);
    espconn_regist_time(s->espconn, 1500, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_listen_obj, esp_socket_listen);

// method socket.accept()
STATIC mp_obj_t esp_socket_accept(mp_obj_t self_in) {
    esp_socket_obj_t *s = self_in;

    if (s->connlist == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "not listening"));
    }

    do {
        mp_uint_t len;
        mp_obj_t *items;

        mp_obj_list_get(s->connlist, &len, &items);
        if (len == 0) {
            break;
        }

        esp_socket_obj_t *rs = items[0];
        mp_obj_list_remove(s->connlist, rs);
        if (rs->espconn->state != ESPCONN_CLOSE) {
            return rs;
        }
    } while (true);

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
        "no connection in queue"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_socket_accept_obj, esp_socket_accept);

// method socket.connect(address)
STATIC mp_obj_t esp_socket_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    esp_socket_obj_t *s = self_in;

    if (s->espconn == NULL || s->espconn->state != ESPCONN_NONE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "transport endpoint is already connected or closed"));
    }

    espconn_regist_connectcb(s->espconn, esp_socket_connect_callback_client);
    espconn_regist_recvcb(s->espconn, esp_socket_recv_callback);
    espconn_regist_sentcb(s->espconn, esp_socket_sent_callback);
    espconn_regist_disconcb(s->espconn, esp_socket_disconnect_callback);

    s->espconn->proto.tcp->remote_port =
        netutils_parse_inet_addr(addr_in, s->espconn->proto.tcp->remote_ip,
            NETUTILS_BIG);
    espconn_connect(s->espconn);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_connect_obj, esp_socket_connect);

// method socket.send(bytes)
STATIC mp_obj_t esp_socket_send(mp_obj_t self_in, mp_obj_t buf_in) {
    esp_socket_obj_t *s = self_in;

    if (s->espconn->state == ESPCONN_NONE || s->espconn->state == ESPCONN_CLOSE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "not connected"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    espconn_sent(s->espconn, bufinfo.buf, bufinfo.len);

    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_send_obj, esp_socket_send);

// method socket.recv(bufsize)
STATIC mp_obj_t esp_socket_recv(mp_obj_t self_in, mp_obj_t len_in) {
    esp_socket_obj_t *s = self_in;

    if (s->recvbuf == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "no data available"));
    }

    mp_uint_t mxl = mp_obj_get_int(len_in);
    if (mxl >= s->recvbuf_len) {
        mp_obj_t trt = mp_obj_new_bytes(s->recvbuf, s->recvbuf_len);
        m_del(uint8_t, s->recvbuf, s->recvbuf_len);
        s->recvbuf = NULL;
        return trt;
    } else {
        mp_obj_t trt = mp_obj_new_bytes(s->recvbuf, mxl);
        memmove(s->recvbuf, &s->recvbuf[mxl], s->recvbuf_len - mxl);
        s->recvbuf = m_renew(uint8_t, s->recvbuf, s->recvbuf_len, s->recvbuf_len - mxl);
        s->recvbuf_len -= mxl;
        return trt;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_recv_obj, esp_socket_recv);

// method socket.sendto(bytes, address)
STATIC mp_obj_t esp_socket_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "UDP not supported"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_socket_sendto_obj, esp_socket_sendto);

// method socket.recvfrom(bufsize)
STATIC mp_obj_t esp_socket_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "UDP not supported"));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_recvfrom_obj, esp_socket_recvfrom);

// method socket.getpeername()
STATIC mp_obj_t esp_socket_getpeername(mp_obj_t self_in) {
    esp_socket_obj_t *s = self_in;

    if (s->espconn->state == ESPCONN_NONE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "not connected"));
    }

    mp_obj_t tuple[2] = {
        netutils_format_ipv4_addr(s->espconn->proto.tcp->remote_ip, NETUTILS_BIG),
        mp_obj_new_int(s->espconn->proto.tcp->remote_port),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_socket_getpeername_obj, esp_socket_getpeername);

STATIC mp_obj_t esp_socket_onconnect(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_socket_obj_t *s = self_in;
    s->cb_connect = lambda_in;

    if (s->connlist != NULL) {
        do {
            mp_uint_t len;
            mp_obj_t *items;

            mp_obj_list_get(s->connlist, &len, &items);
            if (len == 0) {
                break;
            }

            esp_socket_obj_t *rs = items[0];
            mp_obj_list_remove(s->connlist, rs);
            if (s->espconn->state != ESPCONN_CLOSE) {
                call_function_1_protected(s->cb_connect, rs);
            }
        } while (true);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_onconnect_obj, esp_socket_onconnect);

STATIC mp_obj_t esp_socket_onrecv(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_socket_obj_t *s = self_in;
    s->cb_recv = lambda_in;
    if (s->recvbuf != NULL) {
        call_function_2_protected(s->cb_recv, s,
            mp_obj_new_bytes((byte *)s->recvbuf, s->recvbuf_len));
        s->recvbuf = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_onrecv_obj, esp_socket_onrecv);

STATIC mp_obj_t esp_socket_onsent(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_socket_obj_t *s = self_in;
    s->cb_sent = lambda_in;
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_onsent_obj, esp_socket_onsent);

STATIC mp_obj_t esp_socket_ondisconnect(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_socket_obj_t *s = self_in;
    s->cb_disconnect = lambda_in;

    if (s->espconn->state == ESPCONN_CLOSE) {
        call_function_1_protected(s->cb_disconnect, s);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_socket_ondisconnect_obj, esp_socket_ondisconnect);

typedef struct _esp_getaddrinfo_cb_struct_t {
    mp_obj_t lambda;
    mp_uint_t port;
} esp_getaddrinfo_cb_struct_t;

STATIC esp_getaddrinfo_cb_struct_t esp_getaddrinfo_cb_struct;

STATIC void esp_getaddrinfo_cb(const char *name, ip_addr_t *ipaddr, void *arg) {
    mp_obj_t namestr = mp_obj_new_str(name, strlen(name), true);
    if (ipaddr != NULL) {
        uint8_t ip[4];
        ip[0] = (ipaddr->addr >> 24) & 0xff;
        ip[1] = (ipaddr->addr >> 16) & 0xff;
        ip[2] = (ipaddr->addr >>  8) & 0xff;
        ip[3] = (ipaddr->addr >>  0) & 0xff;

        mp_obj_tuple_t *tuple = mp_obj_new_tuple(5, NULL);

        tuple->items[0] = MP_OBJ_NEW_SMALL_INT(0);
        tuple->items[1] = MP_OBJ_NEW_SMALL_INT(0);
        tuple->items[2] = MP_OBJ_NEW_SMALL_INT(0);
        tuple->items[3] = MP_OBJ_NEW_QSTR(MP_QSTR_);
        tuple->items[4] = netutils_format_inet_addr(ip,
            esp_getaddrinfo_cb_struct.port, NETUTILS_LITTLE);
        call_function_2_protected(esp_getaddrinfo_cb_struct.lambda, namestr, tuple);
    } else {
        call_function_2_protected(esp_getaddrinfo_cb_struct.lambda, namestr, mp_const_none);
    }
}

STATIC mp_obj_t esp_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in,
    mp_obj_t lambda_in) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    ip_addr_t ipaddr;

    esp_getaddrinfo_cb_struct.lambda = lambda_in;
    esp_getaddrinfo_cb_struct.port = mp_obj_get_int(port_in);

    err_t ret = espconn_gethostbyname(NULL, host, &ipaddr,
        esp_getaddrinfo_cb);

    if (ret == ESPCONN_OK) {
        esp_getaddrinfo_cb(host, &ipaddr, NULL);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_getaddrinfo_obj, esp_getaddrinfo);

STATIC const mp_map_elem_t esp_socket_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&esp_socket___del___obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close), (mp_obj_t)&esp_socket_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind), (mp_obj_t)&esp_socket_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen), (mp_obj_t)&esp_socket_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept), (mp_obj_t)&esp_socket_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_socket_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), (mp_obj_t)&esp_socket_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), (mp_obj_t)&esp_socket_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto), (mp_obj_t)&esp_socket_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom), (mp_obj_t)&esp_socket_recvfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getpeername), (mp_obj_t)&esp_socket_getpeername_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_onconnect), (mp_obj_t)&esp_socket_onconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_onrecv), (mp_obj_t)&esp_socket_onrecv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_onsent), (mp_obj_t)&esp_socket_onsent_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ondisconnect), (mp_obj_t)&esp_socket_ondisconnect_obj },
};
STATIC MP_DEFINE_CONST_DICT(esp_socket_locals_dict, esp_socket_locals_dict_table);

STATIC const mp_obj_type_t esp_socket_type = {
    { &mp_type_type },
    .name = MP_QSTR_socket,
    .make_new = esp_socket_make_new,
    .locals_dict = (mp_obj_t)&esp_socket_locals_dict,
};
#endif

#define MODESP_INCLUDE_CONSTANTS (1)

void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, msg));
    }
}

STATIC mp_obj_t esp_osdebug(mp_obj_t val) {
    if (val == mp_const_none) {
        uart_os_config(-1);
    } else {
        uart_os_config(mp_obj_get_int(val));
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_osdebug_obj, esp_osdebug);

STATIC mp_obj_t esp_sleep_type(mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int(wifi_get_sleep_type());
    } else {
        wifi_set_sleep_type(mp_obj_get_int(args[0]));
        return mp_const_none;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_sleep_type_obj, 0, 1, esp_sleep_type);

STATIC mp_obj_t esp_deepsleep(mp_uint_t n_args, const mp_obj_t *args) {
    uint32_t sleep_us = n_args > 0 ? mp_obj_get_int(args[0]) : 0;
    // prepare for RTC reset at wake up
    rtc_prepare_deepsleep(sleep_us);
    system_deep_sleep_set_option(n_args > 1 ? mp_obj_get_int(args[1]) : 0);
    system_deep_sleep(sleep_us);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp_deepsleep_obj, 0, 2, esp_deepsleep);

STATIC mp_obj_t esp_flash_id() {
    return mp_obj_new_int(spi_flash_get_id());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_id_obj, esp_flash_id);

STATIC mp_obj_t esp_flash_read(mp_obj_t offset_in, mp_obj_t len_or_buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);

    mp_int_t len;
    byte *buf;
    bool alloc_buf = MP_OBJ_IS_INT(len_or_buf_in);

    if (alloc_buf) {
        len = mp_obj_get_int(len_or_buf_in);
        buf = m_new(byte, len);
    } else {
        mp_buffer_info_t bufinfo;
        mp_get_buffer_raise(len_or_buf_in, &bufinfo, MP_BUFFER_WRITE);
        len = bufinfo.len;
        buf = bufinfo.buf;
    }

    // We know that allocation will be 4-byte aligned for sure
    SpiFlashOpResult res = spi_flash_read(offset, (uint32_t*)buf, len);
    if (res == SPI_FLASH_RESULT_OK) {
        if (alloc_buf) {
            return mp_obj_new_bytes(buf, len);
        }
        return mp_const_none;
    }
    if (alloc_buf) {
        m_del(byte, buf, len);
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_read_obj, esp_flash_read);

STATIC mp_obj_t esp_flash_write(mp_obj_t offset_in, const mp_obj_t buf_in) {
    mp_int_t offset = mp_obj_get_int(offset_in);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len & 0x3) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "len must be multiple of 4"));
    }
    SpiFlashOpResult res = spi_flash_write(offset, bufinfo.buf, bufinfo.len);
    if (res == SPI_FLASH_RESULT_OK) {
        return mp_const_none;
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_flash_write_obj, esp_flash_write);

STATIC mp_obj_t esp_flash_erase(mp_obj_t sector_in) {
    mp_int_t sector = mp_obj_get_int(sector_in);
    SpiFlashOpResult res = spi_flash_erase_sector(sector);
    if (res == SPI_FLASH_RESULT_OK) {
        return mp_const_none;
    }
    mp_raise_OSError(res == SPI_FLASH_RESULT_TIMEOUT ? MP_ETIMEDOUT : MP_EIO);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_flash_erase_obj, esp_flash_erase);

STATIC mp_obj_t esp_flash_size(void) {
    extern char flashchip;
    // For SDK 1.5.2, either address has shifted and not mirrored in
    // eagle.rom.addr.v6.ld, or extra initial member was added.
    SpiFlashChip *flash = (SpiFlashChip*)(&flashchip + 4);
    #if 0
    printf("deviceId: %x\n", flash->deviceId);
    printf("chip_size: %u\n", flash->chip_size);
    printf("block_size: %u\n", flash->block_size);
    printf("sector_size: %u\n", flash->sector_size);
    printf("page_size: %u\n", flash->page_size);
    printf("status_mask: %u\n", flash->status_mask);
    #endif
    return mp_obj_new_int_from_uint(flash->chip_size);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_flash_size_obj, esp_flash_size);

STATIC mp_obj_t esp_check_fw(void) {
    MD5_CTX ctx;
    uint32_t *sz_p = (uint32_t*)0x40208ffc;
    printf("size: %d\n", *sz_p);
    MD5Init(&ctx);
    MD5Update(&ctx, (char*)0x40200004, *sz_p - 4);
    unsigned char digest[16];
    MD5Final(digest, &ctx);
    printf("md5: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x", digest[i]);
    }
    printf("\n");
    return mp_obj_new_bool(memcmp(digest, (void*)(0x40200000 + *sz_p), sizeof(digest)) == 0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_check_fw_obj, esp_check_fw);


STATIC mp_obj_t esp_neopixel_write_(mp_obj_t pin, mp_obj_t buf, mp_obj_t is800k) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    esp_neopixel_write(mp_obj_get_pin_obj(pin)->phys_port,
        (uint8_t*)bufinfo.buf, bufinfo.len, mp_obj_is_true(is800k));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_neopixel_write_obj, esp_neopixel_write_);

#if MICROPY_ESP8266_APA102
STATIC mp_obj_t esp_apa102_write_(mp_obj_t clockPin, mp_obj_t dataPin, mp_obj_t buf) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf, &bufinfo, MP_BUFFER_READ);
    esp_apa102_write(mp_obj_get_pin_obj(clockPin)->phys_port,
        mp_obj_get_pin_obj(dataPin)->phys_port,
        (uint8_t*)bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_apa102_write_obj, esp_apa102_write_);
#endif

STATIC mp_obj_t esp_freemem() {
    return MP_OBJ_NEW_SMALL_INT(system_get_free_heap_size());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_freemem_obj, esp_freemem);

STATIC mp_obj_t esp_meminfo() {
    system_print_meminfo();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_meminfo_obj, esp_meminfo);

STATIC mp_obj_t esp_malloc(mp_obj_t size_in) {
    return MP_OBJ_NEW_SMALL_INT((mp_uint_t)os_malloc(mp_obj_get_int(size_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_malloc_obj, esp_malloc);

STATIC mp_obj_t esp_free(mp_obj_t addr_in) {
    os_free((void*)mp_obj_get_int(addr_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_free_obj, esp_free);

STATIC mp_obj_t esp_esf_free_bufs(mp_obj_t idx_in) {
    return MP_OBJ_NEW_SMALL_INT(ets_esf_free_bufs(mp_obj_get_int(idx_in)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_esf_free_bufs_obj, esp_esf_free_bufs);

STATIC const mp_map_elem_t esp_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_esp) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_osdebug), (mp_obj_t)&esp_osdebug_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sleep_type), (mp_obj_t)&esp_sleep_type_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deepsleep), (mp_obj_t)&esp_deepsleep_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_id), (mp_obj_t)&esp_flash_id_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_read), (mp_obj_t)&esp_flash_read_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_write), (mp_obj_t)&esp_flash_write_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_erase), (mp_obj_t)&esp_flash_erase_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_flash_size), (mp_obj_t)&esp_flash_size_obj },
    #if MODESP_ESPCONN
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket), (mp_obj_t)&esp_socket_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&esp_getaddrinfo_obj },
    #endif
    #if MICROPY_ESP8266_NEOPIXEL
    { MP_OBJ_NEW_QSTR(MP_QSTR_neopixel_write), (mp_obj_t)&esp_neopixel_write_obj },
    #endif
    #if MICROPY_ESP8266_APA102
    { MP_OBJ_NEW_QSTR(MP_QSTR_apa102_write), (mp_obj_t)&esp_apa102_write_obj },
    #endif
    { MP_OBJ_NEW_QSTR(MP_QSTR_dht_readinto), (mp_obj_t)&dht_readinto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_freemem), (mp_obj_t)&esp_freemem_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_meminfo), (mp_obj_t)&esp_meminfo_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_check_fw), (mp_obj_t)&esp_check_fw_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_info), (mp_obj_t)&pyb_info_obj }, // TODO delete/rename/move elsewhere
    { MP_OBJ_NEW_QSTR(MP_QSTR_malloc), (mp_obj_t)&esp_malloc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_free), (mp_obj_t)&esp_free_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_esf_free_bufs), (mp_obj_t)&esp_esf_free_bufs_obj },

#if MODESP_INCLUDE_CONSTANTS
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_NONE),
        MP_OBJ_NEW_SMALL_INT(NONE_SLEEP_T) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_LIGHT),
        MP_OBJ_NEW_SMALL_INT(LIGHT_SLEEP_T) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_SLEEP_MODEM),
        MP_OBJ_NEW_SMALL_INT(MODEM_SLEEP_T) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_STA_MODE),
        MP_OBJ_NEW_SMALL_INT(STATION_MODE)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_AP_MODE),
        MP_OBJ_NEW_SMALL_INT(SOFTAP_MODE)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_STA_AP_MODE),
        MP_OBJ_NEW_SMALL_INT(STATIONAP_MODE)},
#endif
};

STATIC MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp_module_globals,
};
