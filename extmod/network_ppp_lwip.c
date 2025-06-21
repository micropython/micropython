/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Damien P. George
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

// This file is intended to closely match ports/esp32/network_ppp.c. Changes can
// and should probably be applied to both files. Compare them directly by using:
// git diff --no-index extmod/network_ppp_lwip.c ports/esp32/network_ppp.c

#include "py/runtime.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "extmod/modnetwork.h"

#if MICROPY_PY_NETWORK_PPP_LWIP

#include "lwip/dns.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/pppapi.h"
#include "netif/ppp/pppos.h"

// Enable this to see the serial data going between the PPP layer.
#define PPP_TRACE_IN_OUT (0)

typedef enum {
    STATE_INACTIVE,
    STATE_ACTIVE,
    STATE_ERROR,
    STATE_CONNECTING,
    STATE_CONNECTED,
} network_ppp_state_t;

typedef struct _network_ppp_obj_t {
    mp_obj_base_t base;
    network_ppp_state_t state;
    int error_code;
    mp_obj_t stream;
    ppp_pcb *pcb;
    struct netif netif;
} network_ppp_obj_t;

const mp_obj_type_t mp_network_ppp_lwip_type;

static mp_obj_t network_ppp___del__(mp_obj_t self_in);

static void network_ppp_stream_uart_irq_disable(network_ppp_obj_t *self) {
    if (self->stream == mp_const_none) {
        return;
    }

    // Disable UART IRQ.
    mp_obj_t dest[3];
    mp_load_method(self->stream, MP_QSTR_irq, dest);
    dest[2] = mp_const_none;
    mp_call_method_n_kw(1, 0, dest);
}

static void network_ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx) {
    network_ppp_obj_t *self = ctx;
    switch (err_code) {
        case PPPERR_NONE:
            self->state = STATE_CONNECTED;
            break;
        case PPPERR_USER:
            if (self->state >= STATE_ERROR) {
                // Indicate that we are no longer connected and thus
                // only need to free the PPP PCB, not close it.
                self->state = STATE_ACTIVE;
            }
            // Clean up the PPP PCB.
            network_ppp___del__(MP_OBJ_FROM_PTR(self));
            break;
        default:
            self->state = STATE_ERROR;
            self->error_code = err_code;
            break;
    }
}

static mp_obj_t network_ppp_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_t stream = all_args[0];

    if (stream != mp_const_none) {
        mp_get_stream_raise(stream, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE);
    }

    network_ppp_obj_t *self = mp_obj_malloc_with_finaliser(network_ppp_obj_t, type);
    self->state = STATE_INACTIVE;
    self->stream = stream;
    self->pcb = NULL;

    return MP_OBJ_FROM_PTR(self);
}

static mp_obj_t network_ppp___del__(mp_obj_t self_in) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->state >= STATE_ACTIVE) {
        if (self->state >= STATE_ERROR) {
            // Still connected over the stream.
            // Force the connection to close, with nocarrier=1.
            self->state = STATE_INACTIVE;
            ppp_close(self->pcb, 1);
        }
        network_ppp_stream_uart_irq_disable(self);
        // Free PPP PCB and reset state.
        self->state = STATE_INACTIVE;
        ppp_free(self->pcb);
        self->pcb = NULL;
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ppp___del___obj, network_ppp___del__);

static mp_obj_t network_ppp_poll(size_t n_args, const mp_obj_t *args) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->state <= STATE_ERROR) {
        return MP_OBJ_NEW_SMALL_INT(-MP_EPERM);
    }

    mp_int_t total_len = 0;
    mp_obj_t stream = self->stream;
    while (stream != mp_const_none) {
        uint8_t buf[256];
        int err;
        mp_uint_t len = mp_stream_rw(stream, buf, sizeof(buf), &err, 0);
        if (len == 0) {
            break;
        }
        #if PPP_TRACE_IN_OUT
        mp_printf(&mp_plat_print, "ppp_in(n=%u,data=", len);
        for (size_t i = 0; i < len; ++i) {
            mp_printf(&mp_plat_print, "%02x:", buf[i]);
        }
        mp_printf(&mp_plat_print, ")\n");
        #endif
        pppos_input(self->pcb, (u8_t *)buf, len);
        total_len += len;
    }

    return MP_OBJ_NEW_SMALL_INT(total_len);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ppp_poll_obj, 1, 2, network_ppp_poll);

static void network_ppp_stream_uart_irq_enable(network_ppp_obj_t *self) {
    if (self->stream == mp_const_none) {
        return;
    }

    // Enable UART IRQ to call PPP.poll() when incoming data is ready.
    mp_obj_t dest[4];
    mp_load_method(self->stream, MP_QSTR_irq, dest);
    dest[2] = mp_obj_new_bound_meth(MP_OBJ_FROM_PTR(&network_ppp_poll_obj), MP_OBJ_FROM_PTR(self));
    dest[3] = mp_load_attr(self->stream, MP_QSTR_IRQ_RXIDLE);
    mp_call_method_n_kw(2, 0, dest);
}

static mp_obj_t network_ppp_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("either pos or kw args are allowed"));
    }
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (kwargs->used != 0) {
        for (size_t i = 0; i < kwargs->alloc; i++) {
            if (mp_map_slot_is_filled(kwargs, i)) {
                switch (mp_obj_str_get_qstr(kwargs->table[i].key)) {
                    case MP_QSTR_stream: {
                        if (kwargs->table[i].value != mp_const_none) {
                            mp_get_stream_raise(kwargs->table[i].value, MP_STREAM_OP_READ | MP_STREAM_OP_WRITE);
                        }
                        if (self->state >= STATE_ACTIVE) {
                            network_ppp_stream_uart_irq_disable(self);
                        }
                        self->stream = kwargs->table[i].value;
                        if (self->state >= STATE_ACTIVE) {
                            network_ppp_stream_uart_irq_enable(self);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
        }
        return mp_const_none;
    }

    if (n_args != 2) {
        mp_raise_TypeError(MP_ERROR_TEXT("can query only one param"));
    }

    mp_obj_t val = mp_const_none;

    switch (mp_obj_str_get_qstr(args[1])) {
        case MP_QSTR_stream: {
            val = self->stream;
            break;
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
    }

    return val;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ppp_config_obj, 1, network_ppp_config);

static mp_obj_t network_ppp_status(mp_obj_t self_in) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->state == STATE_ERROR) {
        return MP_OBJ_NEW_SMALL_INT(-self->error_code);
    } else {
        return MP_OBJ_NEW_SMALL_INT(self->state);
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ppp_status_obj, network_ppp_status);

static u32_t network_ppp_output_callback(ppp_pcb *pcb, const void *data, u32_t len, void *ctx) {
    network_ppp_obj_t *self = ctx;
    #if PPP_TRACE_IN_OUT
    mp_printf(&mp_plat_print, "ppp_out(n=%u,data=", len);
    for (size_t i = 0; i < len; ++i) {
        mp_printf(&mp_plat_print, "%02x:", ((const uint8_t *)data)[i]);
    }
    mp_printf(&mp_plat_print, ")\n");
    #endif
    mp_obj_t stream = self->stream;
    if (stream == mp_const_none) {
        return 0;
    }
    int err;
    // The return value from this output callback is the number of bytes written out.
    // If it's less than the requested number of bytes then lwIP will propagate out an error.
    return mp_stream_rw(stream, (void *)data, len, &err, MP_STREAM_RW_WRITE);
}

static mp_obj_t network_ppp_connect(size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    enum { ARG_security, ARG_user, ARG_key };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_security, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = PPPAUTHTYPE_NONE} },
        { MP_QSTR_user, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
        { MP_QSTR_key, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_rom_obj = MP_ROM_NONE} },
    };

    mp_arg_val_t parsed_args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, parsed_args);

    network_ppp_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (self->state == STATE_INACTIVE) {
        self->pcb = pppos_create(&self->netif, network_ppp_output_callback, network_ppp_status_cb, self);
        if (self->pcb == NULL) {
            mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("pppos_create failed"));
        }
        self->state = STATE_ACTIVE;

        network_ppp_stream_uart_irq_enable(self);
    }

    if (self->state == STATE_CONNECTING || self->state == STATE_CONNECTED) {
        mp_raise_OSError(MP_EALREADY);
    }

    switch (parsed_args[ARG_security].u_int) {
        case PPPAUTHTYPE_NONE:
        case PPPAUTHTYPE_PAP:
        case PPPAUTHTYPE_CHAP:
            break;
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("invalid auth"));
    }

    if (parsed_args[ARG_security].u_int != PPPAUTHTYPE_NONE) {
        const char *user_str = mp_obj_str_get_str(parsed_args[ARG_user].u_obj);
        const char *key_str = mp_obj_str_get_str(parsed_args[ARG_key].u_obj);
        ppp_set_auth(self->pcb, parsed_args[ARG_security].u_int, user_str, key_str);
    }

    ppp_set_default(self->pcb);

    ppp_set_usepeerdns(self->pcb, true);

    if (ppp_connect(self->pcb, 0) != ERR_OK) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("ppp_connect failed"));
    }

    self->state = STATE_CONNECTING;

    // Do a poll in case there is data waiting on the input stream.
    network_ppp_poll(1, args);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ppp_connect_obj, 1, network_ppp_connect);

static mp_obj_t network_ppp_disconnect(mp_obj_t self_in) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->state == STATE_CONNECTING || self->state == STATE_CONNECTED) {
        // Initiate close and wait for PPPERR_USER callback.
        ppp_close(self->pcb, 0);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ppp_disconnect_obj, network_ppp_disconnect);

static mp_obj_t network_ppp_isconnected(mp_obj_t self_in) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->state == STATE_CONNECTED);
}
static MP_DEFINE_CONST_FUN_OBJ_1(network_ppp_isconnected_obj, network_ppp_isconnected);

static mp_obj_t network_ppp_ifconfig(size_t n_args, const mp_obj_t *args) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ifconfig(&self->netif, n_args - 1, args + 1);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(network_ppp_ifconfig_obj, 1, 2, network_ppp_ifconfig);

static mp_obj_t network_ppp_ipconfig(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    network_ppp_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    return mod_network_nic_ipconfig(&self->netif, n_args - 1, args + 1, kwargs);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(network_ppp_ipconfig_obj, 1, network_ppp_ipconfig);

static const mp_rom_map_elem_t network_ppp_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&network_ppp___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&network_ppp_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&network_ppp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&network_ppp_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&network_ppp_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&network_ppp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&network_ppp_ifconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_ipconfig), MP_ROM_PTR(&network_ppp_ipconfig_obj) },
    { MP_ROM_QSTR(MP_QSTR_poll), MP_ROM_PTR(&network_ppp_poll_obj) },

    { MP_ROM_QSTR(MP_QSTR_SEC_NONE), MP_ROM_INT(PPPAUTHTYPE_NONE) },
    { MP_ROM_QSTR(MP_QSTR_SEC_PAP), MP_ROM_INT(PPPAUTHTYPE_PAP) },
    { MP_ROM_QSTR(MP_QSTR_SEC_CHAP), MP_ROM_INT(PPPAUTHTYPE_CHAP) },
};
static MP_DEFINE_CONST_DICT(network_ppp_locals_dict, network_ppp_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_ppp_lwip_type,
    MP_QSTR_PPP,
    MP_TYPE_FLAG_NONE,
    make_new, network_ppp_make_new,
    locals_dict, &network_ppp_locals_dict
    );

#endif
