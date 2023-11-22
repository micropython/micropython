/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
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

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#include "mp_usbd.h"

#ifndef NO_QSTR
#include "device/dcd.h"
#endif

#if !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

void mp_usbd_task(void) {
    tud_task_ext(0, false);
}

void mp_usbd_task_callback(mp_sched_node_t *node) {
    (void)node;
    mp_usbd_task();
}

#endif // !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

extern void __real_dcd_event_handler(dcd_event_t const *event, bool in_isr);

// If -Wl,--wrap=dcd_event_handler is passed to the linker, then this wrapper
// will be called and allows MicroPython to schedule the TinyUSB task when
// dcd_event_handler() is called from an ISR.
TU_ATTR_FAST_FUNC void __wrap_dcd_event_handler(dcd_event_t const *event, bool in_isr) {
    __real_dcd_event_handler(event, in_isr);
    mp_usbd_schedule_task();
}

TU_ATTR_FAST_FUNC void mp_usbd_schedule_task(void) {
    static mp_sched_node_t usbd_task_node;
    mp_sched_schedule_node(&usbd_task_node, mp_usbd_task_callback);
}

void mp_usbd_hex_str(char *out_str, const uint8_t *bytes, size_t bytes_len) {
    size_t hex_len = bytes_len * 2;
    for (int i = 0; i < hex_len; i += 2) {
        static const char *hexdig = "0123456789abcdef";
        out_str[i] = hexdig[bytes[i / 2] >> 4];
        out_str[i + 1] = hexdig[bytes[i / 2] & 0x0f];
    }
    out_str[hex_len] = 0;
}

#endif // MICROPY_HW_ENABLE_USBDEV
