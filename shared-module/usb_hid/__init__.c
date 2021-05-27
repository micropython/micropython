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
#include "shared-module/usb_hid/Device.h"
#include "supervisor/memory.h"
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
    0x00,        //  7 bInterfaceProtocol: NONE
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

static supervisor_allocation *hid_report_descriptor_allocation;
static usb_hid_device_obj_t hid_devices[MAX_HID_DEVICES];
// If 0, USB HID is disabled.
static mp_int_t num_hid_devices;

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

bool usb_hid_enabled(void) {
    return num_hid_devices > 0;
}

void usb_hid_set_defaults(void) {
    common_hal_usb_hid_enable(
        CIRCUITPY_USB_HID_ENABLED_DEFAULT ? &default_hid_devices_tuple : mp_const_empty_tuple);
}

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_descriptor_length(void) {
    return sizeof(usb_hid_descriptor_template);
}

static const char usb_hid_interface_name[] = USB_INTERFACE_NAME " HID";

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_add_descriptor(uint8_t *descriptor_buf, descriptor_counts_t *descriptor_counts, uint8_t *current_interface_string, uint16_t report_descriptor_length) {
    memcpy(descriptor_buf, usb_hid_descriptor_template, sizeof(usb_hid_descriptor_template));

    descriptor_buf[HID_DESCRIPTOR_INTERFACE_INDEX] = descriptor_counts->current_interface;
    descriptor_counts->current_interface++;

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
    return common_hal_usb_hid_enable(mp_const_empty_tuple);
}

bool common_hal_usb_hid_enable(const mp_obj_t devices) {
    // We can't change the devices once we're connected.
    if (tud_connected()) {
        return false;
    }

    const mp_int_t num_devices = MP_OBJ_SMALL_INT_VALUE(mp_obj_len(devices));
    if (num_devices > MAX_HID_DEVICES) {
        mp_raise_ValueError_varg(translate("No more than %d HID devices allowed"), MAX_HID_DEVICES);
    }

    num_hid_devices = num_devices;

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

    // Don't need space for a report id if there's only one device.
    if (num_hid_devices == 1) {
        total_hid_report_descriptor_length -= 2;
    }
    return total_hid_report_descriptor_length;
}

// Build the combined HID report descriptor in the given space.
void usb_hid_build_report_descriptor(uint8_t *report_descriptor_space, size_t report_descriptor_length) {
    if (!usb_hid_enabled()) {
        return;
    }

    uint8_t *report_descriptor_start = report_descriptor_space;

    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_obj_t *device = &hid_devices[i];
        // Copy the report descriptor for this device.
        if (num_hid_devices == 1) {
            // There's only one device, so it shouldn't have a report ID.
            // Copy the descriptor, but splice out the report id indicator and value (2 bytes).
            memcpy(report_descriptor_start, device->report_descriptor, device->report_id_index - 1);
            report_descriptor_start += device->report_id_index - 1;
            memcpy(report_descriptor_start, device->report_descriptor + device->report_id_index + 1,
                device->report_descriptor_length - device->report_id_index - 1);
        } else {
            // Copy the whole descriptor and fill in the report id.
            memcpy(report_descriptor_start, device->report_descriptor, device->report_descriptor_length);
            report_descriptor_start[device->report_id_index] = i + 1;

            // Remember the report id that was assigned.
            device->report_id = i + 1;

            // Advance to the next free chunk for the next report descriptor.x
            report_descriptor_start += device->report_descriptor_length;
        }
        // Clear the heap pointer to the bytes of the descriptor.
        // We don't need it any more and it will get lost when the heap goes away.
        device->report_descriptor = NULL;
    }
}

// Call this after the heap and VM are finished.
void usb_hid_save_report_descriptor(uint8_t *report_descriptor_space, size_t report_descriptor_length) {
    if (!usb_hid_enabled()) {
        return;
    }

    // Allocate storage that persists across VMs to hold the combined report descriptor.
    // and to remember the device details.

    // Copy the descriptor from the temporary area to a supervisor storage allocation that
    // will leave between VM instantiations.
    hid_report_descriptor_allocation =
        allocate_memory(align32_size(report_descriptor_length),
            /*high_address*/ false, /*movable*/ false);
    memcpy((uint8_t *)hid_report_descriptor_allocation->ptr, report_descriptor_space, report_descriptor_length);
}

void usb_hid_gc_collect(void) {
    gc_collect_ptr(hid_devices_tuple);

    // Mark possible heap pointers in the static device list as in use.
    for (mp_int_t i = 0; i < num_hid_devices; i++) {
        // Cast away the const for .report_descriptor. It could be in flash or on the heap.
        // Constant report descriptors must be const so that they are used directly from flash
        // and not copied into RAM.
        gc_collect_ptr((void *)hid_devices[i].report_descriptor);
        gc_collect_ptr(hid_devices[i].in_report_buffer);
        gc_collect_ptr(hid_devices[i].out_report_buffer);
    }
}

usb_hid_device_obj_t *usb_hid_get_device_with_report_id(uint8_t report_id) {
    for (uint8_t i = 0; i < num_hid_devices; i++) {
        usb_hid_device_obj_t *device = &hid_devices[i];
        if (device->report_id == report_id) {
            return &hid_devices[i];
        }
    }
    return NULL;
}

// Invoked when GET HID REPORT DESCRIPTOR is received.
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
    return (uint8_t *)hid_report_descriptor_allocation->ptr;
}
