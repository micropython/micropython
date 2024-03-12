/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
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
#include <string.h>
#include <sys/time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

#include "py/obj.h"
#include "py/objstr.h"
#include "py/stream.h"
#include "py/mpstate.h"
#include "py/mphal.h"
#include "extmod/misc.h"
#include "shared/timeutils/timeutils.h"
#include "shared/runtime/pyexec.h"
#include "mphalport.h"
#include "usb.h"
#include "usb_serial_jtag.h"
#include "uart.h"

TaskHandle_t mp_main_task_handle;

static uint8_t stdin_ringbuf_array[260];
ringbuf_t stdin_ringbuf = {stdin_ringbuf_array, sizeof(stdin_ringbuf_array), 0, 0};

// Check the ESP-IDF error code and raise an OSError if it's not ESP_OK.
#if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_NORMAL
void check_esp_err_(esp_err_t code)
#else
void check_esp_err_(esp_err_t code, const char *func, const int line, const char *file)
#endif
{
    if (code != ESP_OK) {
        // map esp-idf error code to posix error code
        uint32_t pcode = -code;
        switch (code) {
            case ESP_ERR_NO_MEM:
                pcode = MP_ENOMEM;
                break;
            case ESP_ERR_TIMEOUT:
                pcode = MP_ETIMEDOUT;
                break;
            case ESP_ERR_NOT_SUPPORTED:
                pcode = MP_EOPNOTSUPP;
                break;
        }
        // construct string object
        mp_obj_str_t *o_str = m_new_obj_maybe(mp_obj_str_t);
        if (o_str == NULL) {
            mp_raise_OSError(pcode);
            return;
        }
        o_str->base.type = &mp_type_str;
        #if MICROPY_ERROR_REPORTING > MICROPY_ERROR_REPORTING_NORMAL
        char err_msg[64];
        esp_err_to_name_r(code, err_msg, sizeof(err_msg));
        vstr_t vstr;
        vstr_init(&vstr, 80);
        vstr_printf(&vstr, "0x%04X %s in function '%s' at line %d in file '%s'", code, err_msg, func, line, file);
        o_str->data = (const byte *)vstr_null_terminated_str(&vstr);
        #else
        o_str->data = (const byte *)esp_err_to_name(code); // esp_err_to_name ret's ptr to const str
        #endif
        o_str->len = strlen((char *)o_str->data);
        o_str->hash = qstr_compute_hash(o_str->data, o_str->len);
        // raise
        mp_obj_t args[2] = { MP_OBJ_NEW_SMALL_INT(pcode), MP_OBJ_FROM_PTR(o_str)};
        nlr_raise(mp_obj_exception_make_new(&mp_type_OSError, 2, 0, args));
    }
}

uintptr_t mp_hal_stdio_poll(uintptr_t poll_flags) {
    uintptr_t ret = 0;
    #if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_poll_rx();
    #endif
    if ((poll_flags & MP_STREAM_POLL_RD) && stdin_ringbuf.iget != stdin_ringbuf.iput) {
        ret |= MP_STREAM_POLL_RD;
    }
    if (poll_flags & MP_STREAM_POLL_WR) {
        ret |= MP_STREAM_POLL_WR;
    }
    return ret;
}

int mp_hal_stdin_rx_chr(void) {
    for (;;) {
        #if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
        usb_serial_jtag_poll_rx();
        #endif
        int c = ringbuf_get(&stdin_ringbuf);
        if (c != -1) {
            return c;
        }
        MICROPY_EVENT_POLL_HOOK
    }
}

mp_uint_t mp_hal_stdout_tx_strn(const char *str, size_t len) {
    // Only release the GIL if many characters are being sent
    mp_uint_t ret = len;
    bool did_write = false;
    bool release_gil = len > 20;
    if (release_gil) {
        MP_THREAD_GIL_EXIT();
    }
    #if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    usb_serial_jtag_tx_strn(str, len);
    did_write = true;
    #elif CONFIG_USB_OTG_SUPPORTED
    usb_tx_strn(str, len);
    did_write = true;
    #endif
    #if MICROPY_HW_ENABLE_UART_REPL
    uart_stdout_tx_strn(str, len);
    did_write = true;
    #endif
    if (release_gil) {
        MP_THREAD_GIL_ENTER();
    }
    int dupterm_res = mp_os_dupterm_tx_strn(str, len);
    if (dupterm_res >= 0) {
        did_write = true;
        ret = MIN((mp_uint_t)dupterm_res, ret);
    }
    return did_write ? ret : 0;
}

uint32_t mp_hal_ticks_ms(void) {
    return esp_timer_get_time() / 1000;
}

uint32_t mp_hal_ticks_us(void) {
    return esp_timer_get_time();
}

void mp_hal_delay_ms(uint32_t ms) {
    uint64_t us = (uint64_t)ms * 1000ULL;
    uint64_t dt;
    uint64_t t0 = esp_timer_get_time();
    for (;;) {
        mp_handle_pending(true);
        MICROPY_PY_SOCKET_EVENTS_HANDLER
        MP_THREAD_GIL_EXIT();
        uint64_t t1 = esp_timer_get_time();
        dt = t1 - t0;
        if (dt + portTICK_PERIOD_MS * 1000ULL >= us) {
            // doing a vTaskDelay would take us beyond requested delay time
            taskYIELD();
            MP_THREAD_GIL_ENTER();
            t1 = esp_timer_get_time();
            dt = t1 - t0;
            break;
        } else {
            ulTaskNotifyTake(pdFALSE, 1);
            MP_THREAD_GIL_ENTER();
        }
    }
    if (dt < us) {
        // do the remaining delay accurately
        mp_hal_delay_us(us - dt);
    }
}

void mp_hal_delay_us(uint32_t us) {
    // these constants are tested for a 240MHz clock
    const uint32_t this_overhead = 5;
    const uint32_t pend_overhead = 150;

    // return if requested delay is less than calling overhead
    if (us < this_overhead) {
        return;
    }
    us -= this_overhead;

    uint64_t t0 = esp_timer_get_time();
    for (;;) {
        uint64_t dt = esp_timer_get_time() - t0;
        if (dt >= us) {
            return;
        }
        if (dt + pend_overhead < us) {
            // we have enough time to service pending events
            // (don't use MICROPY_EVENT_POLL_HOOK because it also yields)
            mp_handle_pending(true);
        }
    }
}

uint64_t mp_hal_time_ns(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t ns = tv.tv_sec * 1000000000ULL;
    ns += (uint64_t)tv.tv_usec * 1000ULL;
    return ns;
}

// Wake up the main task if it is sleeping.
void mp_hal_wake_main_task(void) {
    xTaskNotifyGive(mp_main_task_handle);
}

// Wake up the main task if it is sleeping, to be called from an ISR.
void mp_hal_wake_main_task_from_isr(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    vTaskNotifyGiveFromISR(mp_main_task_handle, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR();
    }
}
