/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Dan Halbert for Adafruit Industries
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

#include "py/gc.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "py/objtuple.h"
#include "shared-bindings/usb_cdc/__init__.h"
#include "shared-bindings/usb_cdc/Serial.h"
#include "supervisor/usb.h"

#include "tusb.h"

#if CFG_TUD_CDC != 2
#error CFG_TUD_CDC must be exactly 2
#endif

static const uint8_t usb_cdc_descriptor_template[] = {
    // CDC IAD Descriptor
    0x08,        //  0 bLength
    0x0B,        //  1 bDescriptorType: IAD Descriptor
    0xFF,        //  2 bFirstInterface  [SET AT RUNTIME]
#define CDC_FIRST_INTERFACE_INDEX 2
    0x02,        //  3 bInterfaceCount: 2
    0x02,        //  4 bFunctionClass: COMM
    0x02,        //  5 bFunctionSubclass: ACM
    0x00,        //  6 bFunctionProtocol: NONE
    0x00,        //  7 iFunction

    // CDC Comm Interface Descriptor
    0x09,        //  8 bLength
    0x04,        //  9 bDescriptorType (Interface)
    0xFF,        // 10 bInterfaceNumber  [SET AT RUNTIME]
#define CDC_COMM_INTERFACE_INDEX 10
    0x00,        // 11 bAlternateSetting
    0x01,        // 12 bNumEndpoints 1
    0x02,        // 13 bInterfaceClass: COMM
    0x02,        // 14 bInterfaceSubClass: ACM
    0x00,        // 15 bInterfaceProtocol: NONE
    0xFF,        // 16 iInterface (String Index)
#define CDC_COMM_INTERFACE_STRING_INDEX 16

    // CDC Header Descriptor
    0x05,        // 17 bLength
    0x24,        // 18 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x00,        // 19 bDescriptorSubtype: NONE
    0x10, 0x01,  // 20,21 bcdCDC: 1.10

    // CDC Call Management Descriptor
    0x05,        // 22 bLength
    0x24,        // 23 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x01,        // 24 bDescriptorSubtype: CALL MANAGEMENT
    0x01,        // 25 bmCapabilities
    0xFF,        // 26 bDataInterface  [SET AT RUNTIME]
#define CDC_CALL_MANAGEMENT_DATA_INTERFACE_INDEX 26

    // CDC Abstract Control Management Descriptor
    0x04,        // 27 bLength
    0x24,        // 28 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x02,        // 29 bDescriptorSubtype: ABSTRACT CONTROL MANAGEMENT
    0x02,        // 30 bmCapabilities

    // CDC Union Descriptor
    0x05,        // 31 bLength
    0x24,        // 32 bDescriptorType: CLASS SPECIFIC INTERFACE
    0x06,        // 33 bDescriptorSubtype: CDC
    0xFF,        // 34 bMasterInterface  [SET AT RUNTIME]
#define CDC_UNION_MASTER_INTERFACE_INDEX 34
    0xFF,        // 35 bSlaveInterface_list (1 item)
#define CDC_UNION_SLAVE_INTERFACE_INDEX 35

    // CDC Control IN Endpoint Descriptor
    0x07,        // 36 bLength
    0x05,        // 37 bDescriptorType (Endpoint)
    0xFF,        // 38 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | number]
#define CDC_CONTROL_IN_ENDPOINT_INDEX 38
    0x03,        // 39 bmAttributes (Interrupt)
    0x40, 0x00,  // 40, 41 wMaxPacketSize 64
    0x10,        // 42 bInterval 16 (unit depends on device speed)

    // CDC Data Interface
    0x09,        // 43 bLength
    0x04,        // 44 bDescriptorType (Interface)
    0xFF,        // 45 bInterfaceNumber  [SET AT RUNTIME]
#define CDC_DATA_INTERFACE_INDEX 45
    0x00,        // 46 bAlternateSetting
    0x02,        // 47 bNumEndpoints 2
    0x0A,        // 48 bInterfaceClass: DATA
    0x00,        // 49 bInterfaceSubClass: NONE
    0x00,        // 50 bInterfaceProtocol
    0x05,        // 51 iInterface (String Index)
#define CDC_DATA_INTERFACE_STRING_INDEX 51

    // CDC Data OUT Endpoint Descriptor
    0x07,        // 52 bLength
    0x05,        // 53 bDescriptorType (Endpoint)
    0xFF,        // 54 bEndpointAddress (OUT/H2D) [SET AT RUNTIME]
#define CDC_DATA_OUT_ENDPOINT_INDEX 54
    0x02,        // 55 bmAttributes (Bulk)
    #if USB_HIGHSPEED
    0x00, 0x02,  // 56,57  wMaxPacketSize 512
    #else
    0x40, 0x00,  // 56,57  wMaxPacketSize 64
    #endif
    0x00,        // 58 bInterval 0 (unit depends on device speed)

    // CDC Data IN Endpoint Descriptor
    0x07,        // 59 bLength
    0x05,        // 60 bDescriptorType (Endpoint)
    0xFF,        // 61 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | number]
#define CDC_DATA_IN_ENDPOINT_INDEX 61
    0x02,        // 62 bmAttributes (Bulk)
    #if USB_HIGHSPEED
    0x00, 0x02,  // 63,64 wMaxPacketSize 512
    #else
    0x40, 0x00,  // 63,64 wMaxPacketSize 64
    #endif
    0x00,        // 65 bInterval 0 (unit depends on device speed)
};

static const char console_cdc_comm_interface_name[] = USB_INTERFACE_NAME " CDC control";
static const char data_cdc_comm_interface_name[] = USB_INTERFACE_NAME " CDC2 control";
static const char console_cdc_data_interface_name[] = USB_INTERFACE_NAME " CDC data";
static const char data_cdc_data_interface_name[] = USB_INTERFACE_NAME " CDC2 data";

// .idx is set later.

static usb_cdc_serial_obj_t usb_cdc_console_obj = {
    .base.type = &usb_cdc_serial_type,
    .timeout = -1.0f,
    .write_timeout = -1.0f,
};

static usb_cdc_serial_obj_t usb_cdc_data_obj = {
    .base.type = &usb_cdc_serial_type,
    .timeout = -1.0f,
    .write_timeout = -1.0f,
};

static bool usb_cdc_console_is_enabled;
static bool usb_cdc_data_is_enabled;

void usb_cdc_set_defaults(void) {
    common_hal_usb_cdc_enable(CIRCUITPY_USB_CDC_CONSOLE_ENABLED_DEFAULT,
        CIRCUITPY_USB_CDC_DATA_ENABLED_DEFAULT);
}

bool usb_cdc_console_enabled(void) {
    return usb_cdc_console_is_enabled;
}

bool usb_cdc_data_enabled(void) {
    return usb_cdc_data_is_enabled;
}

size_t usb_cdc_descriptor_length(void) {
    return sizeof(usb_cdc_descriptor_template);
}

size_t usb_cdc_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string, bool console) {
    memcpy(descriptor_buf, usb_cdc_descriptor_template, sizeof(usb_cdc_descriptor_template));

    // Store comm interface number.
    descriptor_buf[CDC_FIRST_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_buf[CDC_COMM_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_buf[CDC_UNION_MASTER_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    // Now store data interface number.
    descriptor_buf[CDC_CALL_MANAGEMENT_DATA_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_buf[CDC_UNION_SLAVE_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_buf[CDC_DATA_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    descriptor_buf[CDC_CONTROL_IN_ENDPOINT_INDEX] = 0x80 | (
        console
        ? (USB_CDC_EP_NUM_NOTIFICATION ? USB_CDC_EP_NUM_NOTIFICATION : descriptor_counts->current_endpoint)
        : (USB_CDC2_EP_NUM_NOTIFICATION ? USB_CDC2_EP_NUM_NOTIFICATION : descriptor_counts->current_endpoint));
    descriptor_counts->num_in_endpoints++;
    descriptor_counts->current_endpoint++;

    descriptor_buf[CDC_DATA_IN_ENDPOINT_INDEX] = 0x80 | (
        console
        ? (USB_CDC_EP_NUM_DATA_IN ? USB_CDC_EP_NUM_DATA_IN : descriptor_counts->current_endpoint)
        : (USB_CDC2_EP_NUM_DATA_IN ? USB_CDC2_EP_NUM_DATA_IN : descriptor_counts->current_endpoint));
    descriptor_counts->num_in_endpoints++;
    descriptor_buf[CDC_DATA_OUT_ENDPOINT_INDEX] =
        console
        ? (USB_CDC_EP_NUM_DATA_OUT ? USB_CDC_EP_NUM_DATA_OUT : descriptor_counts->current_endpoint)
        : (USB_CDC2_EP_NUM_DATA_OUT ? USB_CDC2_EP_NUM_DATA_OUT : descriptor_counts->current_endpoint);
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;

    usb_add_interface_string(*current_interface_string,
        console ? console_cdc_comm_interface_name : data_cdc_comm_interface_name);
    descriptor_buf[CDC_COMM_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    usb_add_interface_string(*current_interface_string,
        console ? console_cdc_data_interface_name : data_cdc_data_interface_name);
    descriptor_buf[CDC_DATA_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    return sizeof(usb_cdc_descriptor_template);
}

bool common_hal_usb_cdc_disable(void) {
    return common_hal_usb_cdc_enable(false, false);
}

bool common_hal_usb_cdc_enable(bool console, bool data) {
    // We can't change the descriptors once we're connected.
    if (tud_connected()) {
        return false;
    }

    // Right now these objects contain no heap objects, but if that changes,
    // they will need to be protected against gc.

    // Assign only as many idx values as necessary. They must start at 0.
    uint8_t idx = 0;
    usb_cdc_console_is_enabled = console;
    usb_cdc_set_console(console ? MP_OBJ_FROM_PTR(&usb_cdc_console_obj) : mp_const_none);
    if (console) {
        usb_cdc_console_obj.idx = idx;
        idx++;
    }

    usb_cdc_data_is_enabled = data;
    usb_cdc_set_data(data ? MP_OBJ_FROM_PTR(&usb_cdc_data_obj) : mp_const_none);
    if (data) {
        usb_cdc_data_obj.idx = idx;
    }


    return true;
}
