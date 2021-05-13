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

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/usb_hid/Device.h"
#include "shared-module/usb_hid/__init__.h"
#include "shared-module/usb_hid/Device.h"
#include "supervisor/shared/translate.h"
#include "supervisor/shared/tick.h"
#include "tusb.h"

static const uint8_t keyboard_report_descriptor[] = {
    0x05, 0x01,            // 0,1 Usage Page (Generic Desktop Ctrls)
    0x09, 0x06,            // 2,3 Usage (Keyboard)
    0xA1, 0x01,            // 4,5 Collection (Application)
    0x85, 0xFF,            //   6,7 Report ID  [SET AT RUNTIME]
#define KEYBOARD_REPORT_ID_INDEX (7)
    0x05, 0x07,            //   Usage Page (Kbrd/Keypad)
    0x19, 0xE0,            //   Usage Minimum (0xE0)
    0x29, 0xE7,            //   Usage Maximum (0xE7)
    0x15, 0x00,            //   Logical Minimum (0)
    0x25, 0x01,            //   Logical Maximum (1)
    0x75, 0x01,            //   Report Size (1)
    0x95, 0x08,            //   Report Count (8)
    0x81, 0x02,            //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x81, 0x01,            //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x19, 0x00,            //   Usage Minimum (0x00)
    0x29, 0xDD,            //   Usage Maximum (0xDD)
    0x15, 0x00,            //   Logical Minimum (0)
    0x25, 0xDD,            //   Logical Maximum (-35)
    0x75, 0x08,            //   Report Size (8)
    0x95, 0x06,            //   Report Count (6)
    0x81, 0x00,            //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x08,            //   Usage Page (LEDs)
    0x19, 0x01,            //   Usage Minimum (Num Lock)
    0x29, 0x05,            //   Usage Maximum (Kana)
    0x15, 0x00,            //   Logical Minimum (0)
    0x25, 0x01,            //   Logical Maximum (1)
    0x75, 0x01,            //   Report Size (1)
    0x95, 0x05,            //   Report Count (5)
    0x91, 0x02,            //   Output (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0x95, 0x03,            //   Report Count (3)
    0x91, 0x01,            //   Output (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
    0xC0,                  // End Collection
};

const usb_hid_device_obj_t usb_hid_device_keyboard_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = keyboard_report_descriptor,
    .report_descriptor_length = sizeof(keyboard_report_descriptor),
    .usage_page = 0x01,
    .usage = 0x06,
    .in_report_length = 8,
    .out_report_length = 1,
    .report_id_index = KEYBOARD_REPORT_ID_INDEX,

};

static const uint8_t mouse_report_descriptor[] = {
    0x05, 0x01,        // 0,1 Usage Page (Generic Desktop Ctrls)
    0x09, 0x02,        // 2,3 Usage (Mouse)
    0xA1, 0x01,        // 4,5 Collection (Application)
    0x09, 0x01,        //   6,7 Usage (Pointer)
    0xA1, 0x00,        //   8,9 Collection (Physical)
    0x85, 0xFF,        //     10, 11 Report ID  [SET AT RUNTIME]
#define MOUSE_REPORT_ID_INDEX (11)
    0x05, 0x09,        //     Usage Page (Button)
    0x19, 0x01,        //     Usage Minimum (0x01)
    0x29, 0x05,        //     Usage Maximum (0x05)
    0x15, 0x00,        //     Logical Minimum (0)
    0x25, 0x01,        //     Logical Maximum (1)
    0x95, 0x05,        //     Report Count (5)
    0x75, 0x01,        //     Report Size (1)
    0x81, 0x02,        //     Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x95, 0x01,        //     Report Count (1)
    0x75, 0x03,        //     Report Size (3)
    0x81, 0x01,        //     Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0x05, 0x01,        //     Usage Page (Generic Desktop Ctrls)
    0x09, 0x30,        //     Usage (X)
    0x09, 0x31,        //     Usage (Y)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x02,        //     Report Count (2)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0x09, 0x38,        //     Usage (Wheel)
    0x15, 0x81,        //     Logical Minimum (-127)
    0x25, 0x7F,        //     Logical Maximum (127)
    0x75, 0x08,        //     Report Size (8)
    0x95, 0x01,        //     Report Count (1)
    0x81, 0x06,        //     Input (Data,Var,Rel,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              //   End Collection
    0xC0,              // End Collection
};

const usb_hid_device_obj_t usb_hid_device_mouse_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = mouse_report_descriptor,
    .report_descriptor_length = sizeof(mouse_report_descriptor),
    .usage_page = 0x01,
    .usage = 0x02,
    .in_report_length = 4,
    .out_report_length = 0,
    .report_id_index = MOUSE_REPORT_ID_INDEX,
};

static const uint8_t consumer_control_report_descriptor[] = {
    0x05, 0x0C,        // 0,1 Usage Page (Consumer)
    0x09, 0x01,        // 2,3 Usage (Consumer Control)
    0xA1, 0x01,        // 4,5 Collection (Application)
    0x85, 0xFF,        //   6,7 Report ID  [SET AT RUNTIME]
#define CONSUMER_CONTROL_REPORT_ID_INDEX (7)
    0x75, 0x10,        //   Report Size (16)
    0x95, 0x01,        //   Report Count (1)
    0x15, 0x01,        //   Logical Minimum (1)
    0x26, 0x8C, 0x02,  //   Logical Maximum (652)
    0x19, 0x01,        //   Usage Minimum (Consumer Control)
    0x2A, 0x8C, 0x02,  //   Usage Maximum (AC Send)
    0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
    0xC0,              // End Collection
};

const usb_hid_device_obj_t usb_hid_device_consumer_control_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = consumer_control_report_descriptor,
    .report_descriptor_length = sizeof(consumer_control_report_descriptor),
    .usage_page = 0x0C,
    .usage = 0x01,
    .in_report_length = 2,
    .out_report_length = 0,
    .report_id_index = CONSUMER_CONTROL_REPORT_ID_INDEX,
};


void common_hal_usb_hid_device_construct(usb_hid_device_obj_t *self, mp_obj_t report_descriptor, uint8_t usage_page, uint8_t usage, uint8_t in_report_length, uint8_t out_report_length, uint8_t report_id_index) {
    // report buffer pointers are NULL at start, and are created when USB is initialized.

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(report_descriptor, &bufinfo, MP_BUFFER_READ);
    self->report_descriptor_length = bufinfo.len;

    // Copy the raw the descriptor bytes into a heap obj. We don't keep the Python descriptor object.

    uint8_t *descriptor_bytes = gc_alloc(bufinfo.len, false, false);
    memcpy(descriptor_bytes, bufinfo.buf, bufinfo.len);
    self->report_descriptor = descriptor_bytes;

    self->usage_page = usage_page;
    self->usage = usage;
    self->in_report_length = in_report_length;
    self->out_report_length = out_report_length;
    self->report_id_index = report_id_index;
}

uint8_t common_hal_usb_hid_device_get_usage_page(usb_hid_device_obj_t *self) {
    return self->usage_page;
}

uint8_t common_hal_usb_hid_device_get_usage(usb_hid_device_obj_t *self) {
    return self->usage;
}

void common_hal_usb_hid_device_send_report(usb_hid_device_obj_t *self, uint8_t *report, uint8_t len) {
    if (len != self->in_report_length) {
        mp_raise_ValueError_varg(translate("Buffer incorrect size. Should be %d bytes."), self->in_report_length);
    }

    // Wait until interface is ready, timeout = 2 seconds
    uint64_t end_ticks = supervisor_ticks_ms64() + 2000;
    while ((supervisor_ticks_ms64() < end_ticks) && !tud_hid_ready()) {
        RUN_BACKGROUND_TASKS;
    }

    if (!tud_hid_ready()) {
        mp_raise_msg(&mp_type_OSError,  translate("USB busy"));
    }

    memcpy(self->in_report_buffer, report, len);

    if (!tud_hid_report(self->report_id, self->in_report_buffer, len)) {
        mp_raise_msg(&mp_type_OSError, translate("USB error"));
    }
}


void usb_hid_device_create_report_buffers(usb_hid_device_obj_t *self) {
    if (self->in_report_length > 0) {
        self->in_report_buffer = gc_alloc(self->in_report_length, false, true /*long-lived*/);
    }
    if (self->out_report_length > 0) {
        self->out_report_buffer = gc_alloc(self->out_report_length, false, true /*long-lived*/);
    }
}


// Callbacks invoked when receive Get_Report request through control endpoint
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    (void)itf;
    // only support Input Report
    if (report_type != HID_REPORT_TYPE_INPUT) {
        return 0;
    }

    // fill buffer with current report
    memcpy(buffer, usb_hid_get_device_with_report_id(report_id)->in_report_buffer, reqlen);
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

    usb_hid_device_obj_t *hid_device = usb_hid_get_device_with_report_id(report_id);

    if (hid_device && hid_device->out_report_length >= bufsize) {
        memcpy(hid_device->out_report_buffer, buffer, bufsize);
    }
}
