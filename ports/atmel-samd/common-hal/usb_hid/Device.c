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

#include "common-hal/usb_hid/Device.h"

#include "py/runtime.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/usb_hid/Device.h"
#include "genhdr/autogen_usb_descriptor.h"

#include "tick.h"

#include "usb/class/hid/device/hiddf_generic.h"

static uint32_t usb_hid_send_report(usb_hid_device_obj_t *self, uint8_t* report, uint8_t len) {

    int32_t status;

    // Don't get stuck if USB fails in some way; timeout after a while.
    uint64_t end_ticks = ticks_ms + 2000;

    while (ticks_ms < end_ticks) {
        status = usb_d_ep_get_status(self->endpoint, NULL);
        if (status == USB_BUSY) {
            continue;
        }
        if (status == USB_OK) {
            break;
        }
        // Some error. Give up.
        return status;
    }

    // Copy the data only when endpoint is ready to send. The previous
    // buffer load gets zero'd out when transaction completes, so if
    // you copy before it's ready, only zeros will get sent.

    // Prefix with a report id if one is supplied.
    if (self->report_id > 0) {
        self->report_buffer[0] = self->report_id;
        memcpy(&(self->report_buffer[1]), report, len);
        return hiddf_generic_write(self->report_buffer, len + 1);
    } else {
        memcpy(self->report_buffer, report, len);
        return hiddf_generic_write(self->report_buffer, len);
    }

}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t* report, uint8_t len) {
    if (len != self->report_length) {
        mp_raise_ValueError_varg("Buffer incorrect size. Should be %d bytes.", self->report_length);
    }
    int32_t status = usb_hid_send_report(self, report, len);
    if (status != ERR_NONE) {
        mp_raise_msg(&mp_type_OSError, status == USB_BUSY ? "USB Busy" : "USB Error");
    }
}

uint8_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint8_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}


void usb_hid_init() {
}

void usb_hid_reset() {
    // We don't actually reset. We just set a report that is empty to prevent
    // long keypresses and such.
    uint8_t report[USB_HID_MAX_REPORT_LENGTH] = {0};

    for (size_t i = 0; i < USB_HID_NUM_DEVICES; i++) {
        usb_hid_send_report(&usb_hid_devices[i], report, usb_hid_devices[i].report_length);
    }
}
