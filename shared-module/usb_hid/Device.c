/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "py/runtime.h"
#include "shared-bindings/usb_hid/Device.h"
#include "shared-module/usb_hid/Device.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/tick.h"
#include "tusb.h"

uint8_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint8_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t *report, uint8_t len) {
    if (len != self->report_length) {
        mp_raise_ValueError_varg(translate("Buffer incorrect size. Should be %d bytes."), self->report_length);
    }

    // Wait until interface is ready, timeout = 2 seconds
    uint64_t end_ticks = supervisor_ticks_ms64() + 2000;
    while ((supervisor_ticks_ms64() < end_ticks) && !tud_hid_ready()) {
        RUN_BACKGROUND_TASKS;
    }

    if (!tud_hid_ready()) {
        mp_raise_msg(&mp_type_OSError,  translate("USB Busy"));
    }

    memcpy(self->report_buffer, report, len);

    if (!tud_hid_report(self->report_id, self->report_buffer, len)) {
        mp_raise_msg(&mp_type_OSError, translate("USB Error"));
    }
}

static usb_hid_device_obj_t *get_hid_device(uint8_t report_id) {
    for (uint8_t i = 0; i < USB_HID_NUM_DEVICES; i++) {
        if (usb_hid_devices[i].report_id == report_id) {
            return &usb_hid_devices[i];
        }
    }
    return NULL;
}

// Callbacks invoked when receive Get_Report request through control endpoint
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    (void)itf;
    // only support Input Report
    if (report_type != HID_REPORT_TYPE_INPUT) {
        return 0;
    }

    // fill buffer with current report
    memcpy(buffer, get_hid_device(report_id)->report_buffer, reqlen);
    return reqlen;
}

// Callbacks invoked when receive Set_Report request through control endpoint
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    (void)itf;
    if (report_type == HID_REPORT_TYPE_INVALID) {
        report_id = buffer[0];
        buffer++;
        bufsize--;
    } else if (report_type != HID_REPORT_TYPE_OUTPUT) {
        return;
    }

    usb_hid_device_obj_t *hid_device = get_hid_device(report_id);

    if (hid_device && hid_device->out_report_length >= bufsize) {
        memcpy(hid_device->out_report_buffer, buffer, bufsize);
    }
}
