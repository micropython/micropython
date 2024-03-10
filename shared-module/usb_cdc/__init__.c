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

#if CIRCUITPY_USB_VENDOR
#include "supervisor/port.h"
#endif

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

#if CIRCUITPY_USB_VENDOR
#include "usb_vendor_descriptors.h"

#define BOS_TOTAL_LEN      (TUD_BOS_DESC_LEN + TUD_BOS_WEBUSB_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

#define MS_OS_20_DESC_LEN  0xB2

// BOS Descriptor is required for webUSB
uint8_t const desc_bos[] =
{
    // total length, number of device caps
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 2),

    // Vendor Code, iLandingPage
    TUD_BOS_WEBUSB_DESCRIPTOR(VENDOR_REQUEST_WEBUSB, 1),

    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, VENDOR_REQUEST_MICROSOFT)
};

uint8_t const *tud_descriptor_bos_cb(void) {
    return desc_bos;
}

#define MS_OS_20_ITF_NUM_MAGIC 0x5b
#define MS_OS_20_ITF_NUM_OFFSET 22

const uint8_t ms_os_20_descriptor_template[] =
{
    // 10 Set header: length, type, windows version, total length
    U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // 8 Configuration subset header: length, type, configuration index, reserved, configuration total length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION), 0, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A),

    // 8 Function Subset header: length, type, first interface, reserved, subset length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION), /* 22 */ MS_OS_20_ITF_NUM_MAGIC, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),

    // 20 MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
    U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sub-compatible

    // MS OS 2.0 Registry property descriptor: length, type
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08 - 0x08 - 0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
    'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
    U16_TO_U8S_LE(0x0050), // wPropertyDataLength
    // bPropertyData: “{975F44D9-0D08-43FD-8B3E-127CA8AFFF9D}”.
    '{', 0x00, '9', 0x00, '7', 0x00, '5', 0x00, 'F', 0x00, '4', 0x00, '4', 0x00, 'D', 0x00, '9', 0x00, '-', 0x00,
    '0', 0x00, 'D', 0x00, '0', 0x00, '8', 0x00, '-', 0x00, '4', 0x00, '3', 0x00, 'F', 0x00, 'D', 0x00, '-', 0x00,
    '8', 0x00, 'B', 0x00, '3', 0x00, 'E', 0x00, '-', 0x00, '1', 0x00, '2', 0x00, '7', 0x00, 'C', 0x00, 'A', 0x00,
    '8', 0x00, 'A', 0x00, 'F', 0x00, 'F', 0x00, 'F', 0x00, '9', 0x00, 'D', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00
};

TU_VERIFY_STATIC(sizeof(ms_os_20_descriptor_template) == MS_OS_20_DESC_LEN, "Incorrect size");

static const uint8_t usb_vendor_descriptor_template[] = {
    // Vendor Descriptor
    0x09,        // 0 bLength
    0x04,        // 1 bDescriptorType (Interface)
    0xFF,        // 2 bInterfaceNumber  [SET AT RUNTIME]
#define VENDOR_INTERFACE_INDEX 2
    0x00,        // 3 bAlternateSetting
    0x02,        // 4 bNumEndpoints 2
    0xFF,        // 5 bInterfaceClass: Vendor Specific
    0x00,        // 6 bInterfaceSubClass: NONE
    0x00,        // 7 bInterfaceProtocol: NONE
    0xFF,        // 8 iInterface (String Index)
#define VENDOR_INTERFACE_STRING_INDEX 8

    // Vendor OUT Endpoint Descriptor
    0x07,        // 9 bLength
    0x05,        // 10 bDescriptorType (Endpoint)
    0xFF,        // 11 bEndpointAddress (IN/D2H) [SET AT RUNTIME: number]
#define VENDOR_OUT_ENDPOINT_INDEX 11
    0x02,        // 12 bmAttributes (Bulk)
    #if USB_HIGHSPEED
    0x00, 0x02,  // 13,14  wMaxPacketSize 512
    #else
    0x40, 0x00,  // 13,14  wMaxPacketSize 64
    #endif
    0x0,         // 15  bInterval 0

    // Vendor IN Endpoint Descriptor
    0x07,        // 16 bLength
    0x05,        // 17 bDescriptorType (Endpoint)
    0xFF,        // 18 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | number]
#define VENDOR_IN_ENDPOINT_INDEX 18
    0x02,        // 19 bmAttributes (Bulk)
    0x40, 0x00,  // 20, 21 wMaxPacketSize 64
    0x0          // 22  bInterval 0
};

static const char vendor_interface_name[] = USB_INTERFACE_NAME " WebUSB";


bool usb_vendor_enabled(void) {
    return usb_cdc_console_enabled();
}

size_t usb_vendor_descriptor_length(void) {
    return sizeof(usb_vendor_descriptor_template);
}

static uint8_t *ms_os_20_descriptor = NULL;

size_t vendor_ms_os_20_descriptor_length() {
    return ms_os_20_descriptor != NULL ? sizeof(ms_os_20_descriptor_template) : 0;
}
uint8_t const *vendor_ms_os_20_descriptor() {
    return ms_os_20_descriptor;
}

size_t usb_vendor_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string) {
    if (ms_os_20_descriptor == NULL) {
        ms_os_20_descriptor = port_malloc(sizeof(ms_os_20_descriptor_template), false);
        if (ms_os_20_descriptor == NULL) {
            return 0;
        }
        memcpy(ms_os_20_descriptor, ms_os_20_descriptor_template, sizeof(ms_os_20_descriptor_template));
        ms_os_20_descriptor[MS_OS_20_ITF_NUM_OFFSET] = descriptor_counts->current_interface;
        ms_os_20_descriptor[VENDOR_IN_ENDPOINT_INDEX] = 0x80 | descriptor_counts->current_endpoint;
        ms_os_20_descriptor[VENDOR_OUT_ENDPOINT_INDEX] = descriptor_counts->current_endpoint;
    }

    memcpy(descriptor_buf, usb_vendor_descriptor_template, sizeof(usb_vendor_descriptor_template));

    descriptor_buf[VENDOR_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    descriptor_buf[VENDOR_IN_ENDPOINT_INDEX] = 0x80 | descriptor_counts->current_endpoint;
    descriptor_counts->num_in_endpoints++;
    descriptor_buf[VENDOR_OUT_ENDPOINT_INDEX] = descriptor_counts->current_endpoint;
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;

    usb_add_interface_string(*current_interface_string, vendor_interface_name);
    descriptor_buf[VENDOR_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    return sizeof(usb_vendor_descriptor_template);
}

#endif
