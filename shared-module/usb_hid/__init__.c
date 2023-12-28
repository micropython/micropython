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

#include "tusb.h"

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/usb_hid/__init__.h"
#include "shared-bindings/usb_hid/Device.h"
#include "supervisor/port.h"
#include "supervisor/usb.h"

static const uint8_t usb_hid_descriptor_template[] = {
    0x09,        //  0 bLength
    0x04,        //  1 bDescriptorType (Interface)
    0xFF,        //  2 bInterfaceNumber 3
#define HID_DESCRIPTOR_INTERFACE_INDEX (2)
    0x00,        //  3 bAlternateSetting
    0x02,        //  4 bNumEndpoints 2
    0x03,        //  5 bInterfaceClass: HID
    0x00,        //  6 bInterfaceSubClass: NOBOOT
#define HID_DESCRIPTOR_SUBCLASS_INDEX (6)
    0x00,        //  7 bInterfaceProtocol: NONE
#define HID_DESCRIPTOR_INTERFACE_PROTOCOL_INDEX (7)
    0xFF,        //  8 iInterface (String Index)  [SET AT RUNTIME]
#define HID_DESCRIPTOR_INTERFACE_STRING_INDEX (8)

    0x09,        //  9 bLength
    0x21,        // 10 bDescriptorType (HID)
    0x11, 0x01,  // 11,12 bcdHID 1.11
    0x00,        // 13 bCountryCode
    0x01,        // 14 bNumDescriptors
    0x22,        // 15 bDescriptorType[0] (HID)
    0xFF, 0xFF,  // 16,17 wDescriptorLength[0]   [SET AT RUNTIME: lo, hi]
#define HID_DESCRIPTOR_LENGTH_INDEX (16)

    0x07,        // 18 bLength
    0x05,        // 19 bDescriptorType (Endpoint)
    0xFF,        // 20 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | endpoint]
#define HID_IN_ENDPOINT_INDEX (20)
    0x03,        // 21 bmAttributes (Interrupt)
    0x40, 0x00,  // 22,23  wMaxPacketSize 64
    0x08,        // 24 bInterval 8 (unit depends on device speed)

    0x07,        // 25 bLength
    0x05,        // 26 bDescriptorType (Endpoint)
    0xFF,        // 27 bEndpointAddress (OUT/H2D)  [SET AT RUNTIME]
#define HID_OUT_ENDPOINT_INDEX (27)
    0x03,        // 28 bmAttributes (Interrupt)
    0x40, 0x00,  // 29,30 wMaxPacketSize 64
    0x08,        // 31 bInterval 8 (unit depends on device speed)
};

#define MAX_HID_DEVICES 8

static uint8_t *hid_report_descriptor = NULL;
static usb_hid_device_obj_t hid_devices[MAX_HID_DEVICES];
// If 0, USB HID is disabled.
static mp_int_t num_hid_devices;

// Which boot device is available? 0: no boot devices, 1: boot keyboard, 2: boot mouse.
// This value is set by usb_hid.enable(), and used to build the HID interface descriptor.
// The value is remembered here from boot.py to code.py.
static uint8_t hid_boot_device;

// Whether a boot device was requested by a SET_PROTOCOL request from the host.
static bool hid_boot_device_requested;

// This tuple is store in usb_hid.devices.
static mp_obj_tuple_t *hid_devices_tuple;

static mp_obj_tuple_t default_hid_devices_tuple = {
    .base = {
        .type = &mp_type_tuple,
    },
    .len = 3,
    .items = {
        MP_OBJ_FROM_PTR(&usb_hid_device_keyboard_obj),
        MP_OBJ_FROM_PTR(&usb_hid_device_mouse_obj),
        MP_OBJ_FROM_PTR(&usb_hid_device_consumer_control_obj),
    },
};

// These describe the standard descriptors used for boot keyboard and mouse, which don't use report IDs.
// When the host requests a boot device, replace whatever HID devices were enabled with a tuple
// containing just one of these, since the host is uninterested in other devices.
// The driver code will then use the proper report length and send_report() will not send a report ID.
static const usb_hid_device_obj_t boot_keyboard_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = NULL,
    .report_descriptor_length = 0,
    .usage_page = 0x01,
    .usage = 0x06,
    .num_report_ids = 1,
    .report_ids = { 0, },
    .in_report_lengths = { 8, },
    .out_report_lengths = { 1, },
};

static const usb_hid_device_obj_t boot_mouse_obj = {
    .base = {
        .type = &usb_hid_device_type,
    },
    .report_descriptor = NULL,
    .report_descriptor_length = 0,
    .usage_page = 0x01,
    .usage = 0x02,
    .num_report_ids = 1,
    .report_ids = { 0, },
    .in_report_lengths = { 4, },
    .out_report_lengths = { 0, },
};

bool usb_hid_enabled(void) {
    return num_hid_devices > 0;
}

uint8_t usb_hid_boot_device(void) {
    return hid_boot_device;
}

// Returns 1 or 2 if host requested a boot device and boot protocol was enabled in the interface descriptor.
uint8_t common_hal_usb_hid_get_boot_device(void) {
    return hid_boot_device_requested ? hid_boot_device : 0;
}

void usb_hid_set_defaults(void) {
    hid_boot_device = 0;
    hid_boot_device_requested = false;
    common_hal_usb_hid_enable(
        CIRCUITPY_USB_HID_ENABLED_DEFAULT ? &default_hid_devices_tuple : mp_const_empty_tuple, 0);
}

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_descriptor_length(void) {
    return sizeof(usb_hid_descriptor_template);
}

static const char usb_hid_interface_name[] = USB_INTERFACE_NAME " HID";

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string, uint16_t report_descriptor_length, uint8_t boot_device) {
    memcpy(descriptor_buf, usb_hid_descriptor_template, sizeof(usb_hid_descriptor_template));

    descriptor_buf[HID_DESCRIPTOR_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

    if (boot_device > 0) {
        descriptor_buf[HID_DESCRIPTOR_SUBCLASS_INDEX] = 1; // BOOT protocol (device) available.
        descriptor_buf[HID_DESCRIPTOR_INTERFACE_PROTOCOL_INDEX] = boot_device; // 1: keyboard, 2: mouse
    }

    usb_add_interface_string(*current_interface_string, usb_hid_interface_name);
    descriptor_buf[HID_DESCRIPTOR_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX] = report_descriptor_length & 0xFF;
    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX + 1] = (report_descriptor_length >> 8);

    descriptor_buf[HID_IN_ENDPOINT_INDEX] =
        0x80 | (USB_HID_EP_NUM_IN ? USB_HID_EP_NUM_IN : descriptor_counts->current_endpoint);
    descriptor_counts->num_in_endpoints++;
    descriptor_buf[HID_OUT_ENDPOINT_INDEX] =
        USB_HID_EP_NUM_OUT ? USB_HID_EP_NUM_OUT : descriptor_counts->current_endpoint;
    descriptor_counts->num_out_endpoints++;
    descriptor_counts->current_endpoint++;

    return sizeof(usb_hid_descriptor_template);
}

// Make up a fresh tuple containing the device objects saved in the static
// devices table. Save the tuple in usb_hid.devices.
static void usb_hid_set_devices_from_hid_devices(void) {
    mp_obj_t tuple_items[num_hid_devices];
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        tuple_items[i] = &hid_devices[i];
    }

    // Remember tuple for gc purposes.
    hid_devices_tuple = mp_obj_new_tuple(num_hid_devices, tuple_items);
    usb_hid_set_devices(hid_devices_tuple);
}

bool common_hal_usb_hid_disable(void) {
    return common_hal_usb_hid_enable(mp_const_empty_tuple, 0);
}

bool common_hal_usb_hid_enable(const mp_obj_t devices, uint8_t boot_device) {
    // We can't change the devices once we're connected.
    if (tud_connected()) {
        return false;
    }

    const mp_int_t num_devices = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(devices));
    mp_arg_validate_length_max(num_devices, MAX_HID_DEVICES, MP_QSTR_devices);

    num_hid_devices = num_devices;

    hid_boot_device = boot_device;

    // Remember the devices in static storage so they live across VMs.
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        // devices has already been validated to contain only usb_hid_device_obj_t objects.
        usb_hid_device_obj_t *device =
            MP_OBJ_TO_PTR(mp_obj_subscr(devices, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        memcpy(&hid_devices[i], device, sizeof(usb_hid_device_obj_t));
    }

    usb_hid_set_devices_from_hid_devices();

    return true;
}

// Called when HID devices are ready to be used, when code.py or the REPL starts running.
void usb_hid_setup_devices(void) {

    // If the host requested a boot device, replace the current list of devices
    // with a single-element tuple containing the proper boot device.
    if (hid_boot_device_requested) {
        memcpy(&hid_devices[0],
            // Will be 1 (keyboard) or 2 (mouse).
            hid_boot_device == 1 ? &boot_keyboard_obj : &boot_mouse_obj,
            sizeof(usb_hid_device_obj_t));
        num_hid_devices = 1;
    }

    usb_hid_set_devices_from_hid_devices();

    // Create report buffers on the heap.
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_create_report_buffers(&hid_devices[i]);
    }
}

// Total length of the report descriptor, with all configured devices.
size_t usb_hid_report_descriptor_length(void) {
    size_t total_hid_report_descriptor_length = 0;
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        total_hid_report_descriptor_length += hid_devices[i].report_descriptor_length;
    }

    return total_hid_report_descriptor_length;
}

// Build the combined HID report descriptor in the given space.
void usb_hid_build_report_descriptor(void) {
    if (!usb_hid_enabled()) {
        return;
    }
    size_t report_length = usb_hid_report_descriptor_length();
    hid_report_descriptor = port_malloc(report_length, false);
    if (hid_report_descriptor == NULL) {
        return;
    }

    uint8_t *report_descriptor_start = hid_report_descriptor;

    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_obj_t *device = &hid_devices[i];
        // Copy the report descriptor for this device.
        memcpy(report_descriptor_start, device->report_descriptor, device->report_descriptor_length);

        // Advance to the next free chunk for the next report descriptor.x
        report_descriptor_start += device->report_descriptor_length;

        // Clear the heap pointer to the bytes of the descriptor.
        // We don't need it any more and it will get lost when the heap goes away.
        device->report_descriptor = NULL;
    }
}

void usb_hid_gc_collect(void) {
    gc_collect_ptr(hid_devices_tuple);

    // Mark possible heap pointers in the static device list as in use.
    for (mp_int_t device_idx = 0; device_idx < num_hid_devices; device_idx++) {

        // Cast away the const for .report_descriptor. It could be in flash or on the heap.
        // Constant report descriptors must be const so that they are used directly from flash
        // and not copied into RAM.
        gc_collect_ptr((void *)hid_devices[device_idx].report_descriptor);

        // Collect all the report buffers for this device.
        for (size_t id_idx = 0; id_idx < hid_devices[device_idx].num_report_ids; id_idx++) {
            gc_collect_ptr(hid_devices[device_idx].in_report_buffers[id_idx]);
            gc_collect_ptr(hid_devices[device_idx].out_report_buffers[id_idx]);
        }
    }
}

bool usb_hid_get_device_with_report_id(uint8_t report_id, usb_hid_device_obj_t **device_out, size_t *id_idx_out) {
    for (uint8_t device_idx = 0; device_idx < num_hid_devices; device_idx++) {
        usb_hid_device_obj_t *device = &hid_devices[device_idx];
        for (size_t id_idx = 0; id_idx < device->num_report_ids; id_idx++) {
            if (device->report_ids[id_idx] == report_id) {
                *device_out = device;
                *id_idx_out = id_idx;
                return true;
            }
        }
    }
    return false;
}

// Callback invoked when we receive a GET HID REPORT DESCRIPTOR
// Application returns pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
    return (uint8_t *)hid_report_descriptor;
}

// Callback invoked when we receive a SET_PROTOCOL request.
// Protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t instance, uint8_t protocol) {
    hid_boot_device_requested = (protocol == HID_PROTOCOL_BOOT);
}
