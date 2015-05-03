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
#include MICROPY_HAL_H
#include "queue.h"
#include "user_interface.h"
#include "espconn.h"

#include "utils.h"

#define IPV4ADDR_BUFSIZE    4

// Helpers from cc3200/mods/modnetwork.c
STATIC void esp_espconn_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    mp_uint_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_in, &addr_len);
    if (addr_len == 0) {
        // special case of no address given
        memset(out_ip, 0, IPV4ADDR_BUFSIZE);
        return;
    }
    const char *s = addr_str;
    const char *s_top = addr_str + addr_len;
    for (mp_uint_t i = 3 ; ; i--) {
        mp_uint_t val = 0;
        for (; s < s_top && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        out_ip[IPV4ADDR_BUFSIZE - 1 - i] = val;
        if (i == 0 && s == s_top) {
            return;
        } else if (i > 0 && s < s_top && *s == '.') {
            s++;
        } else {
            nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "invalid arguments"));
        }
    }
}

// Takes an address of the form ('192.168.0.1', 8080), returns the port and
// puts IP in out_ip (which must take at least IPADDR_BUF_SIZE bytes).
STATIC mp_uint_t esp_espconn_parse_inet_addr(mp_obj_t addr_in, uint8_t *out_ip) {
    mp_obj_t *addr_items;
    mp_obj_get_array_fixed_n(addr_in, 2, &addr_items);
    esp_espconn_parse_ipv4_addr(addr_items[0], out_ip);
    return mp_obj_get_int(addr_items[1]);
}

// Takes an array with a raw IPv4 address and returns something like '192.168.0.1'.
STATIC mp_obj_t esp_espconn_format_ipv4_addr(uint8_t *ip) {
    char ip_str[16];
    mp_uint_t ip_len = snprintf(ip_str, 16, "%u.%u.%u.%u", ip[0], ip[1], ip[2], ip[3]);
    return mp_obj_new_str(ip_str, ip_len, false);
}

// Takes an array with a raw IP address, and a port, and returns a net-address
// tuple such as ('192.168.0.1', 8080).
STATIC mp_obj_t esp_espconn_format_inet_addr(uint8_t *ip, mp_uint_t port) {
    mp_obj_t tuple[2] = {
        tuple[0] = esp_espconn_format_ipv4_addr(ip),
        tuple[1] = mp_obj_new_int(port),
    };
    return mp_obj_new_tuple(2, tuple);
}

STATIC const mp_obj_type_t esp_espconn_type;

typedef struct _esp_espconn_obj_t {
    mp_obj_base_t base;
    struct espconn *espconn;

    mp_obj_t cb_connect;
    mp_obj_t cb_recv;
    mp_obj_t cb_disconnect;

    uint8_t *recvbuf;
    mp_uint_t recvbuf_len;

    bool server;

    mp_obj_list_t *connlist;
} esp_espconn_obj_t;

// Due to the onconnect callback not being able to recognize the parent esp_espconn,
// we can have only one esp_espconn listening at a time
// This should be solvable by some PIC hacking
STATIC esp_espconn_obj_t *esp_espconn_listening;

STATIC mp_obj_t esp_espconn_make_new_base() {
    esp_espconn_obj_t *s = m_new_obj_with_finaliser(esp_espconn_obj_t);
    s->recvbuf = NULL;
    s->base.type = (mp_obj_t)&esp_espconn_type;
    s->cb_connect = mp_const_none;
    s->cb_recv = mp_const_none;
    s->cb_disconnect = mp_const_none;
    s->server = false;
    return s;
}

// constructor esp_espconn(family=AF_INET, type=SOCK_STREAM, proto=IPPROTO_TCP, fileno=None)
// Arguments ignored as we do not support UDP (yet)
STATIC mp_obj_t esp_espconn_make_new(mp_obj_t type_in, mp_uint_t n_args,
    mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 4, false);

    esp_espconn_obj_t *s = esp_espconn_make_new_base();
    s->espconn = gc_alloc(sizeof(struct espconn), false);
    espconn_create(s->espconn);

    s->espconn->reverse = s;
    // TODO: UDP Support
    s->espconn->type = ESPCONN_TCP;
    s->espconn->state = ESPCONN_NONE;

    s->espconn->proto.tcp = gc_alloc(sizeof(esp_tcp), false);

    return s;
}

// method espconn.close()
STATIC mp_obj_t esp_espconn_close(mp_obj_t self_in) {
    esp_espconn_obj_t *s = self_in;

    if (esp_espconn_listening == s)
        esp_espconn_listening = NULL;

    if (s->espconn->state != ESPCONN_NONE && s->espconn->state != ESPCONN_CLOSE) {
        espconn_disconnect(s->espconn);
    }

    if (s->connlist != NULL) {
        mp_obj_list_set_len(s->connlist, 0);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_espconn_close_obj, esp_espconn_close);

// method socket.__del__()
STATIC mp_obj_t esp_espconn___del__(mp_obj_t self_in) {
    esp_espconn_obj_t *s = self_in;

    esp_espconn_close(self_in);

    if (s->server) {
        espconn_delete(s->espconn);
    } else {
        gc_free(s->espconn->proto.tcp);
        gc_free(s->espconn);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_espconn___del___obj, esp_espconn___del__);

// method espconn.bind(address)
STATIC mp_obj_t esp_espconn_bind(mp_obj_t self_in, mp_obj_t addr_in) {
    esp_espconn_obj_t *s = self_in;

    mp_uint_t port = esp_espconn_parse_inet_addr(addr_in,
        s->espconn->proto.tcp->remote_ip);
    s->espconn->proto.tcp->local_port = port;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_bind_obj, esp_espconn_bind);

STATIC void esp_espconn_recv_callback(void *arg, char *pdata, unsigned short len) {
    struct espconn *conn = arg;
    esp_espconn_obj_t *s = conn->reverse;

    if (s->cb_recv != mp_const_none) {
        call_function_2_protected(s->cb_recv, s, mp_obj_new_bytes((byte *)pdata, len));
    } else {
        if (s->recvbuf == NULL) {
            s->recvbuf = gc_alloc(len, false);
            s->recvbuf_len = len;
            if (s->recvbuf != NULL) {
                memcpy(s->recvbuf, pdata, len);
            }
        } else {
            s->recvbuf = gc_realloc(s->recvbuf, s->recvbuf_len + len);
            if (s->recvbuf != NULL) {
                memcpy(&s->recvbuf[s->recvbuf_len], pdata, len);
                s->recvbuf_len += len;
            }
        }
        if (s->recvbuf == NULL) {
            esp_espconn_close(s);
            return;
        }
    }
}

STATIC void esp_espconn_sent_callback(void *arg) {}

STATIC void esp_espconn_disconnect_callback(void *arg) {
    struct espconn *conn = arg;
    esp_espconn_obj_t *s = conn->reverse;

    if (s->cb_disconnect != mp_const_none) {
        call_function_1_protected(s->cb_disconnect, s);
    }

    esp_espconn_close(s);
}

STATIC void esp_espconn_connect_callback_server(void *arg) {
    struct espconn *conn = arg;

    esp_espconn_obj_t *s = esp_espconn_make_new_base();
    s->espconn = conn;
    conn->reverse = s;

    espconn_regist_recvcb(conn, esp_espconn_recv_callback);
    espconn_regist_sentcb(conn, esp_espconn_sent_callback);
    espconn_regist_disconcb(conn, esp_espconn_disconnect_callback);
    espconn_regist_time(conn, 15, 0);

    if (esp_espconn_listening->cb_connect != mp_const_none) {
        call_function_1_protected(esp_espconn_listening->cb_connect, s);
    } else {
        mp_obj_list_append(esp_espconn_listening->connlist, s);
    }
}

STATIC void esp_espconn_connect_callback_client(void *arg) {
    struct espconn *conn = arg;
    esp_espconn_obj_t *s = conn->reverse;

    if (s->cb_connect != mp_const_none) {
        call_function_1_protected(s->cb_connect, s);
    }
};

// method espconn.listen(backlog)
STATIC mp_obj_t esp_espconn_listen(mp_obj_t self_in, mp_obj_t backlog) {
    esp_espconn_obj_t *s = self_in;

    if (esp_espconn_listening != NULL)
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Only one espconn can listen at a time"));

    esp_espconn_listening = s;

    s->connlist = mp_obj_new_list(0, NULL);
    s->server = true;

    espconn_regist_connectcb(s->espconn, esp_espconn_connect_callback_server);
    espconn_accept(s->espconn);
    espconn_regist_time(s->espconn, 1500, 0);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_listen_obj, esp_espconn_listen);

// method espconn.accept()
STATIC mp_obj_t esp_espconn_accept(mp_obj_t self_in) {
    esp_espconn_obj_t *s = self_in;

    if (s->connlist == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Not listening"));
    }

    do {
        mp_uint_t len;
        mp_obj_t *items;

        mp_obj_list_get(s->connlist, &len, &items);
        if (len == 0) {
            break;
        }

        esp_espconn_obj_t *rs = items[0];
        mp_obj_list_remove(s->connlist, rs);
        if (rs->espconn->state != ESPCONN_CLOSE) {
            return rs;
        }
    } while (true);

    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
        "No connection in queue"));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_espconn_accept_obj, esp_espconn_accept);

// method espconn.connect(address)
STATIC mp_obj_t esp_espconn_connect(mp_obj_t self_in, mp_obj_t addr_in) {
    esp_espconn_obj_t *s = self_in;

    if (s->espconn == NULL || s->espconn->state != ESPCONN_NONE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Transport endpoint is already connected or closed"));
    }

    espconn_regist_connectcb(s->espconn, esp_espconn_connect_callback_client);
    espconn_regist_recvcb(s->espconn, esp_espconn_recv_callback);
    espconn_regist_sentcb(s->espconn, esp_espconn_sent_callback);
    espconn_regist_disconcb(s->espconn, esp_espconn_disconnect_callback);

    s->espconn->proto.tcp->remote_port =
        esp_espconn_parse_inet_addr(addr_in, s->espconn->proto.tcp->remote_ip);
    espconn_connect(s->espconn);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_connect_obj, esp_espconn_connect);

// method espconn.send(bytes)
STATIC mp_obj_t esp_espconn_send(mp_obj_t self_in, mp_obj_t buf_in) {
    esp_espconn_obj_t *s = self_in;

    if (s->espconn->state == ESPCONN_NONE || s->espconn->state == ESPCONN_CLOSE) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "Not connected"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    espconn_sent(s->espconn, bufinfo.buf, bufinfo.len);

    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_send_obj, esp_espconn_send);

// method espconn.recv(bufsize)
STATIC mp_obj_t esp_espconn_recv(mp_obj_t self_in, mp_obj_t len_in) {
    esp_espconn_obj_t *s = self_in;

    if (s->recvbuf == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
            "No data available"));
    }

    mp_uint_t mxl = mp_obj_get_int(len_in);
    if (mxl >= s->recvbuf_len) {
        mp_obj_t trt = mp_obj_new_bytes(s->recvbuf, s->recvbuf_len);
        gc_free(s->recvbuf);
        s->recvbuf = NULL;
        return trt;
    } else {
        mp_obj_t trt = mp_obj_new_bytes(s->recvbuf, mxl);
        memmove(s->recvbuf, &s->recvbuf[mxl], s->recvbuf_len - mxl);
        s->recvbuf_len -= mxl;
        s->recvbuf = gc_realloc(s->recvbuf, s->recvbuf_len);
        return trt;
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_recv_obj, esp_espconn_recv);

// method espconn.sendto(bytes, address)
STATIC mp_obj_t esp_espconn_sendto(mp_obj_t self_in, mp_obj_t data_in, mp_obj_t addr_in) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
        "UDP not supported."));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_espconn_sendto_obj, esp_espconn_sendto);

// method espconn.recvfrom(bufsize)
STATIC mp_obj_t esp_espconn_recvfrom(mp_obj_t self_in, mp_obj_t len_in) {
    nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError,
        "UDP not supported."));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_recvfrom_obj, esp_espconn_recvfrom);

STATIC mp_obj_t esp_espconn_onconnect(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_espconn_obj_t *s = self_in;
    s->cb_connect = lambda_in;

    if (s->connlist != NULL) {
        do {
            mp_uint_t len;
            mp_obj_t *items;

            mp_obj_list_get(s->connlist, &len, &items);
            if (len == 0) {
                break;
            }

            esp_espconn_obj_t *rs = items[0];
            mp_obj_list_remove(s->connlist, rs);
            if (s->espconn->state != ESPCONN_CLOSE) {
                call_function_1_protected(s->cb_connect, rs);
            }
        } while (true);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_onconnect_obj, esp_espconn_onconnect);

STATIC mp_obj_t esp_espconn_onrecv(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_espconn_obj_t *s = self_in;
    s->cb_recv = lambda_in;
    if (s->recvbuf != NULL) {
        call_function_2_protected(s->cb_recv, s,
            mp_obj_new_bytes((byte *)s->recvbuf, s->recvbuf_len));
        s->recvbuf = NULL;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_onrecv_obj, esp_espconn_onrecv);

STATIC mp_obj_t esp_espconn_ondisconnect(mp_obj_t self_in, mp_obj_t lambda_in) {
    esp_espconn_obj_t *s = self_in;
    s->cb_disconnect = lambda_in;

    if (s->espconn->state == ESPCONN_CLOSE) {
        call_function_1_protected(s->cb_disconnect, s);
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_espconn_ondisconnect_obj, esp_espconn_ondisconnect);

STATIC const mp_map_elem_t esp_espconn_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___del__), (mp_obj_t)&esp_espconn___del___obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_close),   (mp_obj_t)&esp_espconn_close_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_bind),    (mp_obj_t)&esp_espconn_bind_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_listen),  (mp_obj_t)&esp_espconn_listen_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_accept),  (mp_obj_t)&esp_espconn_accept_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_espconn_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send),    (mp_obj_t)&esp_espconn_send_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv),    (mp_obj_t)&esp_espconn_recv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sendto),  (mp_obj_t)&esp_espconn_sendto_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recvfrom),    (mp_obj_t)&esp_espconn_recvfrom_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_onconnect),   (mp_obj_t)&esp_espconn_onconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_onrecv),      (mp_obj_t)&esp_espconn_onrecv_obj  },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ondisconnect),(mp_obj_t)&esp_espconn_ondisconnect_obj },
};
STATIC MP_DEFINE_CONST_DICT(esp_espconn_locals_dict, esp_espconn_locals_dict_table);

STATIC const mp_obj_type_t esp_espconn_type = {
    { &mp_type_type },
    .name = MP_QSTR_espconn,
    .make_new = esp_espconn_make_new,
    .locals_dict = (mp_obj_t)&esp_espconn_locals_dict,
};

typedef struct _esp_getaddrinfo_cb_struct_t {
    mp_obj_t lambda;
    mp_uint_t port;
} esp_getaddrinfo_cb_struct_t;

STATIC void esp_getaddrinfo_cb(const char *name, ip_addr_t *ipaddr,
    void *arg) {
    esp_getaddrinfo_cb_struct_t *vals = arg;
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
        tuple->items[4] = esp_espconn_format_inet_addr(ip, vals->port);
        call_function_1_protected(vals->lambda, tuple);
    } else {
        call_function_1_protected(vals->lambda, mp_const_none);
    }
    gc_free(vals);
}

STATIC mp_obj_t esp_getaddrinfo(mp_obj_t host_in, mp_obj_t port_in,
    mp_obj_t lambda_in) {
    mp_uint_t hlen;
    const char *host = mp_obj_str_get_data(host_in, &hlen);
    ip_addr_t ipaddr;

    // Altough esp_espconn_gethostname specifies esp_espconn * as its first argument,
    // it does not look like it actually uses it for anything so we are
    // free to pass whatever we want
    esp_getaddrinfo_cb_struct_t *arg =
        gc_alloc(sizeof(esp_getaddrinfo_cb_struct_t), false);
    arg->port = mp_obj_get_int(port_in);
    arg->lambda = lambda_in;
    err_t ret = espconn_gethostbyname((void *)arg, host, &ipaddr,
        esp_getaddrinfo_cb);

    if (ret == ESPCONN_OK)
        esp_getaddrinfo_cb(host, &ipaddr, (void *)lambda_in);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_3(esp_getaddrinfo_obj, esp_getaddrinfo);

// Singleton instance of scan callback, meaning that there can be only
// one concurrent AP scan.
STATIC mp_obj_t scan_cb_obj;

static void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, msg));
    }
}

STATIC void esp_scan_cb(scaninfo *si, STATUS status) {
    //printf("in pyb_scan_cb: %d, si=%p, si->pbss=%p\n", status, si, si->pbss);
    struct bss_info *bs;
    if (si->pbss) {
        STAILQ_FOREACH(bs, si->pbss, next) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            t->items[0] = mp_obj_new_bytes(bs->ssid, strlen((char*)bs->ssid));
            t->items[1] = mp_obj_new_bytes(bs->bssid, sizeof(bs->bssid));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(bs->channel);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(bs->rssi);
            t->items[4] = MP_OBJ_NEW_SMALL_INT(bs->authmode);
            t->items[5] = MP_OBJ_NEW_SMALL_INT(bs->is_hidden);
            call_function_1_protected(scan_cb_obj, t);
        }
    }
}

STATIC mp_obj_t esp_scan(mp_obj_t cb_in) {
    scan_cb_obj = cb_in;
    wifi_set_opmode(STATION_MODE);
    wifi_station_scan(NULL, (scan_done_cb_t)esp_scan_cb);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(esp_scan_obj, esp_scan);

STATIC mp_obj_t esp_connect(mp_obj_t ssid_in, mp_obj_t passwd_in) {
    struct station_config config = {{0}};
    mp_uint_t len;
    const char *p;

    p = mp_obj_str_get_data(ssid_in, &len);
    memcpy(config.ssid, p, len);
    p = mp_obj_str_get_data(passwd_in, &len);
    memcpy(config.password, p, len);

    error_check(wifi_station_set_config(&config), "Cannot set STA config");
    error_check(wifi_station_connect(), "Cannot connect to AP");

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp_connect_obj, esp_connect);

STATIC mp_obj_t esp_disconnect() {
    error_check(wifi_station_disconnect(), "Cannot disconnect from AP");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_disconnect_obj, esp_disconnect);

STATIC mp_obj_t esp_status() {
    return MP_OBJ_NEW_SMALL_INT(wifi_station_get_connect_status());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(esp_status_obj, esp_status);

STATIC const mp_map_elem_t esp_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_esp) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), (mp_obj_t)&esp_connect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), (mp_obj_t)&esp_disconnect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_scan), (mp_obj_t)&esp_scan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), (mp_obj_t)&esp_status_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_espconn),      (mp_obj_t)&esp_espconn_type },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getaddrinfo), (mp_obj_t)&esp_getaddrinfo_obj },
};

STATIC MP_DEFINE_CONST_DICT(esp_module_globals, esp_module_globals_table);

const mp_obj_module_t esp_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_esp,
    .globals = (mp_obj_dict_t*)&esp_module_globals,
};
