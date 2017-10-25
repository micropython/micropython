/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "py/nlr.h"
#include "common-hal/usb_hid/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/usb_hid/Device.h"

static void report_sent(udd_ep_status_t status, iram_size_t nb_sent,
        udd_ep_id_t ep) {
    UNUSED(status);
    UNUSED(nb_sent);
    for (uint8_t i = 0; i < 2; i++) {
        if (ep == usb_hid_devices[i].endpoint) {
            usb_hid_devices[i].transaction_ongoing = false;
            return;
        }
    }
}

bool usb_hid_send_report(usb_hid_device_obj_t *self, uint8_t* report, uint8_t len) {
    if (!self->enabled) {
        return true;
    }
    // Wait for the previous transaction to finish. Shouldn't happen.
    uint32_t timeout = 0xffff;

    while (self->transaction_ongoing && timeout > 0) {
        timeout--;
    }

    if (self->transaction_ongoing) {
        return false;
    }

    memcpy(self->report_buffer, report, len);

    // Disable interrupts to make sure we save the ongoing state before the
    // report_sent interrupt.
    common_hal_mcu_disable_interrupts();
    bool ok = udd_ep_run(self->endpoint, false,
        self->report_buffer, self->report_length, report_sent);
    self->transaction_ongoing = ok;
    common_hal_mcu_enable_interrupts();
    return ok;
}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t* report, uint8_t len) {
    if (len != self->report_length) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
            "Buffer incorrect size. Should be %d bytes.", self->report_length));
    }
    if (!self->enabled) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "USB Inactive"));
    }
    if (!usb_hid_send_report(self, report, len)) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "USB Busy"));
    }
}

uint8_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint8_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}
