/* * This file is part of the MicroPython project, http://micropython.org/
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


#include "py/runtime.h"
#include "py/mphal.h"
#include "py/objtype.h"
#include "netutils.h"

#include "modmachine.h"
#include "machine_uart.h"

#include "strings.h"

#include "netif/ppp/ppp.h"
#include "netif/ppp/pppos.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "lwip/pppapi.h"

extern const mp_obj_type_t machine_uart_type;

typedef struct _ppp_if_obj_t {
    mp_obj_base_t base;
    bool initialized;
    bool active;
    bool connected;
    ppp_pcb *pcb;
    machine_uart_obj_t* uart;
    struct netif pppif;
    SemaphoreHandle_t inactiveWaitSem;
    TaskHandle_t client_task_handle;
} ppp_if_obj_t;

const mp_obj_type_t ppp_if_type;
STATIC ppp_if_obj_t ppp_obj = {{&ppp_if_type}, false, false, false, NULL, NULL};

/* PPP status callback example */
static void ppp_status_cb(ppp_pcb *pcb, int err_code, void *ctx) // wait for
{
    ppp_if_obj_t* self = &ppp_obj;
    struct netif *pppif = ppp_netif(self->pcb);
    (void)pppif;
    LWIP_UNUSED_ARG(ctx);

    switch (err_code) {
        case PPPERR_NONE: {
                self->connected = (pppif->ip_addr.u_addr.ip4.addr != 0);
                break;
            }
        case PPPERR_USER: {
                xSemaphoreGive(self->inactiveWaitSem);
                break;
            }
        case PPPERR_CONNECT: {
                self->connected = false;
                break;
            }
        default: {
                break;
            }
    }
}

STATIC mp_obj_t get_ppp(mp_obj_t uart) {
    ppp_if_obj_t* self = &ppp_obj;

    if (self->initialized) {
        return MP_OBJ_FROM_PTR(&ppp_obj);
    }

    if (! MP_OBJ_IS_TYPE(uart, &machine_uart_type)) {
        mp_raise_TypeError("requires machine.UART type");
    }

    self->uart = (machine_uart_obj_t*) uart;
    self->inactiveWaitSem = xSemaphoreCreateBinary();
    assert(self->inactiveWaitSem != NULL);
    self->initialized = true;
    return MP_OBJ_FROM_PTR(&ppp_obj);
}
MP_DEFINE_CONST_FUN_OBJ_1(get_ppp_obj, get_ppp);

static u32_t ppp_output_callback(ppp_pcb *pcb, u8_t *data, u32_t len, void *ctx)
{
    ppp_if_obj_t* self = &ppp_obj;
    mp_stream_p_t* uart_stream = (mp_stream_p_t*)self->uart->base.type->protocol;
    int err;

    return uart_stream->write(self->uart, data, len, &err);
}

static void pppos_client_task() {
    ppp_if_obj_t* self = &ppp_obj;
    self->uart->timeout = 50;
    mp_stream_p_t* uart_stream = (mp_stream_p_t*)self->uart->base.type->protocol;
    uint8_t buf[256];

    int err;
    while (ulTaskNotifyTake(pdTRUE, 0) == 0) {
        int len = uart_stream->read(self->uart, buf, sizeof(buf), &err);
        if (len > 0) {
            pppos_input_tcpip(self->pcb, (u8_t *)buf, len);
        }
    }
    vTaskDelete(NULL);
}

STATIC mp_obj_t ppp_active(size_t n_args, const mp_obj_t *args) {
    ppp_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args > 1) {
        if (mp_obj_is_true(args[1])) {
            if (self->active) {
                return mp_const_true;
            }

            self->pcb = pppapi_pppos_create(&self->pppif, ppp_output_callback, ppp_status_cb, NULL);

            if (self->pcb == NULL) {
                mp_raise_msg(&mp_type_RuntimeError, "init failed");
            }
            pppapi_set_default(self->pcb);
            pppapi_connect(self->pcb, 0);

            xTaskCreate(pppos_client_task, "ppp", 1024, NULL, 1, &self->client_task_handle);
            self->active = true;

        } else {
            if (!self->active) {
                return mp_const_false;
            }

            // Wait for PPPERR_USER
            pppapi_close(self->pcb, 0);
            xSemaphoreTake(self->inactiveWaitSem, portMAX_DELAY);
            xSemaphoreGive(self->inactiveWaitSem);

            // Shutdown task
            xTaskNotifyGive(self->client_task_handle);
            while (eTaskGetState(self->client_task_handle) != eDeleted) {
                mp_hal_delay_ms(10);
            }

            pppapi_free(self->pcb);
            self->pcb = NULL;
            self->active = false;
            self->connected = false;
        }
    }
    return mp_obj_new_bool(self->active);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ppp_active_obj, 1, 2, ppp_active);
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
        dns_setserver(0, & dns);
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
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&ppp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&ppp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&ppp_ifconfig_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ppp_if_locals_dict, ppp_if_locals_dict_table);

const mp_obj_type_t ppp_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_PPP,
    .locals_dict = (mp_obj_dict_t*)&ppp_if_locals_dict,
};
