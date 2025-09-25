/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "usbd_hid_interface.h"

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "usb.h"

#if MICROPY_HW_USB_HID

uint8_t *usbd_hid_init(usbd_hid_state_t *hid_in) {
    usbd_hid_itf_t *hid = (usbd_hid_itf_t *)hid_in;
    hid->report_in_len = USBD_HID_REPORT_INVALID;
    return &hid->report_in_buf[0]; // location to place first incoming report
}

int8_t usbd_hid_receive(usbd_hid_state_t *hid_in, size_t len) {
    // Incoming report: save the length but don't schedule next report until user reads this one
    usbd_hid_itf_t *hid = (usbd_hid_itf_t *)hid_in;
    hid->report_in_len = len;
    return USBD_OK;
}

int usbd_hid_rx(usbd_hid_itf_t *hid, size_t len, uint8_t *buf, uint32_t timeout_ms) {
    // Wait for an incoming report
    uint32_t t0 = mp_hal_ticks_ms();
    while (hid->report_in_len == USBD_HID_REPORT_INVALID) {
        if (mp_hal_ticks_ms() - t0 >= timeout_ms || query_irq() == IRQ_STATE_DISABLED) {
            return -MP_ETIMEDOUT;
        }
        MICROPY_EVENT_POLL_HOOK
    }

    // Copy bytes from report to user buffer
    int n = MIN(len, hid->report_in_len);
    memcpy(buf, &hid->report_in_buf[0], n);

    // Schedule to receive next incoming report
    hid->report_in_len = USBD_HID_REPORT_INVALID;
    USBD_HID_ReceivePacket(&hid->base, &hid->report_in_buf[0]);

    // Return number of bytes read into user buffer
    return n;
}

#endif // MICROPY_HW_USB_HID
