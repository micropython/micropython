/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 "Eric Poulsen" <eric@zyxod.com>
 *
 * Based on the ESP IDF example code which is Public Domain / CC0
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

#if !MICROPY_ESP_IDF_4
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objtype.h"
#include "py/stream.h"
#include "netutils.h"
#include "modmachine.h"

#include "netif/ppp/ppp.h"
#include "netif/ppp/pppos.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "netif/ppp/pppapi.h"

#define PPP_CLOSE_TIMEOUT_MS (4000)

typedef struct _ppp_if_obj_t {
    mp_obj_base_t base;
    bool active;
    bool connected;
    volatile bool clean_close;
    ppp_pcb *pcb;
    mp_obj_t stream;
    SemaphoreHandle_t inactiveWaitSem;
    volatile TaskHandle_t client_task_handle;
    struct netif pppif;
} ppp_if_obj_t;

const mp_obj_type_t ppp_if_type;

static void ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx) {
    ppp_if_obj_t* self = ctx;
    struct netif *pppif = ppp_netif(self->pcb);

    switch (err_code) {
        case PPPERR_NONE:
            self->connected = (pppif->ip_addr.u_addr.ip4.addr != 0);
            break;
        case PPPERR_USER:
            self->clean_close = true;
            break;
        case PPPERR_CONNECT:
            self->connected = false;
            break;
        default:
            break;
    }
}

STATIC mp_obj_t ppp_make_new(mp_obj_t stream) {
    mp_get_stream_raise(stream, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE);

    ppp_if_obj_t *self = m_new_obj_with_finaliser(ppp_if_obj_t);

    self->base.type = &ppp_if_type;
    self->stream = stream;
    self->active = false;
    self->connected = false;
    self->clean_close = false;
    self->client_task_handle = NULL;

    return MP_OBJ_FROM_PTR(self);
}
MP_DEFINE_CONST_FUN_OBJ_1(ppp_make_new_obj, ppp_make_new);

static u32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx) {
    ppp_if_obj_t *self = ctx;
    int err;
    return mp_stream_rw(self->stream, data, len, &err, MP_STREAM_RW_WRITE);
}

static void pppos_client_task(void *self_in) {
    ppp_if_obj_t *self = (ppp_if_obj_t*)self_in;
    uint8_t buf[256];

    while (ulTaskNotifyTake(pdTRUE, 0) == 0) {
        int err;
        int len = mp_stream_rw(self->stream, buf, sizeof(buf), &err, 0);
        if (len > 0) {
            pppos_input_tcpip(self->pcb, (u8_t*)buf, len);
        }
    }

    self->client_task_handle = NULL;
    vTaskDelete(NULL);
}

STATIC mp_obj_t ppp_active(size_t n_args, const mp_obj_t *args) {
    ppp_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            if (self->active) {
                return mp_const_true;
            }

            self->pcb = pppapi_pppos_create(&self->pppif, ppp_output_callback, ppp_status_cb, self);

            if (self->pcb == NULL) {
                mp_raise_msg(&mp_type_RuntimeError, "init failed");
            }
            self->active = true;
        } else {
            if (!self->active) {
                return mp_const_false;
            }

            if (self->client_task_handle != NULL) { // is connecting or connected?
                // Wait for PPPERR_USER, with timeout
                pppapi_close(self->pcb, 0);
                uint32_t t0 = mp_hal_ticks_ms();
                while (!self->clean_close && mp_hal_ticks_ms() - t0 < PPP_CLOSE_TIMEOUT_MS) {
                    mp_hal_delay_ms(10);
                }

                // Shutdown task
                xTaskNotifyGive(self->client_task_handle);
                t0 = mp_hal_ticks_ms();
                while (self->client_task_handle != NULL && mp_hal_ticks_ms() - t0 < PPP_CLOSE_TIMEOUT_MS) {
                    mp_hal_delay_ms(10);
                }
            }

            // Release PPP
            pppapi_free(self->pcb);
            self->pcb = NULL;
            self->active = false;
            self->connected = false;
            self->clean_close = false;
        }
    }
    return mp_obj_new_bool(self->active);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ppp_active_obj, 1, 2, ppp_active);

STATIC mp_obj_t ppp_connect_py(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    enum { ARG_authmode, ARG_username, ARG_password };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_authmode, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PPPAUTHTYPE_NONE} },
        { MP_QSTR_username, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_password, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    ppp_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (!self->active) {
        mp_raise_msg(&mp_type_OSError, "must be active");
    }

    if (self->client_task_handle != NULL) {
        mp_raise_OSError(MP_EALREADY);
    }

    switch (parsed_args[ARG_authmode].u_int) {
        case PPPAUTHTYPE_NONE:
        case PPPAUTHTYPE_PAP:
        case PPPAUTHTYPE_CHAP:
            break;
        default:
            mp_raise_msg(&mp_type_ValueError, "invalid auth");
    }

    if (parsed_args[ARG_authmode].u_int != PPPAUTHTYPE_NONE) {
        const char* username_str = mp_obj_str_get_str(parsed_args[ARG_username].u_obj);
        const char* password_str = mp_obj_str_get_str(parsed_args[ARG_password].u_obj);
        pppapi_set_auth(self->pcb, parsed_args[ARG_authmode].u_int, username_str, password_str);
    }
    if (pppapi_set_default(self->pcb) != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, "set default failed");
    }

    ppp_set_usepeerdns(self->pcb, true);

    if (pppapi_connect(self->pcb, 0) != ESP_OK) {
        mp_raise_msg(&mp_type_OSError, "connect failed");
    }

    if (xTaskCreatePinnedToCore(pppos_client_task, "ppp", 2048, self, 1, (TaskHandle_t*)&self->client_task_handle, MP_TASK_COREID) != pdPASS) {
        mp_raise_msg(&mp_type_RuntimeError, "failed to create worker task");
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(ppp_connect_obj, 1, ppp_connect_py);

STATIC mp_obj_t ppp_delete(mp_obj_t self_in) {
    ppp_if_obj_t* self = MP_OBJ_TO_PTR(self_in);
    mp_obj_t args[] = {self, mp_const_false};
    ppp_active(2, args);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(ppp_delete_obj, ppp_delete);

STATIC mp_obj_t ppp_ifconfig(size_t n_args, const mp_obj_t *args) {
    ppp_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    ip_addr_t dns;
    if (n_args == 1) {
        // get
        if (self->pcb != NULL) {
            dns = dns_getserver(0);
            struct netif *pppif = ppp_netif(self->pcb);
            mp_obj_t tuple[4] = {
                netutils_format_ipv4_addr((uint8_t*)&pppif->ip_addr, NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t*)&pppif->gw, NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t*)&pppif->netmask, NETUTILS_BIG),
                netutils_format_ipv4_addr((uint8_t*)&dns, NETUTILS_BIG),
            };
            return mp_obj_new_tuple(4, tuple);
        } else {
            mp_obj_t tuple[4] = { mp_const_none, mp_const_none, mp_const_none, mp_const_none };
            return mp_obj_new_tuple(4, tuple);
        }
    } else {
        mp_obj_t *items;
        mp_obj_get_array_fixed_n(args[1], 4, &items);
        netutils_parse_ipv4_addr(items[3], (uint8_t*)&dns.u_addr.ip4, NETUTILS_BIG);
        dns_setserver(0, &dns);
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ppp_ifconfig_obj, 1, 2, ppp_ifconfig);

STATIC mp_obj_t ppp_status(mp_obj_t self_in) {
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ppp_status_obj, ppp_status);

STATIC mp_obj_t ppp_isconnected(mp_obj_t self_in) {
    ppp_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->connected);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ppp_isconnected_obj, ppp_isconnected);

STATIC const mp_rom_map_elem_t ppp_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&ppp_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&ppp_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&ppp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&ppp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&ppp_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&ppp_delete_obj) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_NONE), MP_ROM_INT(PPPAUTHTYPE_NONE) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_PAP), MP_ROM_INT(PPPAUTHTYPE_PAP) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_CHAP), MP_ROM_INT(PPPAUTHTYPE_CHAP) },
};
STATIC MP_DEFINE_CONST_DICT(ppp_if_locals_dict, ppp_if_locals_dict_table);

const mp_obj_type_t ppp_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_PPP,
    .locals_dict = (mp_obj_dict_t*)&ppp_if_locals_dict,
};

#endif // !MICROPY_ESP_IDF_4
