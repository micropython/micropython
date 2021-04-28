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

// Sequence of devices to configure. Passed to usb_hid.configure_usb().
// Not used after boot.py finishes and VM restarts.
static mp_obj_t hid_devices_seq;

// Is the HID device enabled?
static bool usb_hid_is_enabled;

static supervisor_allocation *hid_report_descriptor_allocation;
static size_t total_hid_report_descriptor_length;
static supervisor_allocation *hid_devices_allocation;
static mp_int_t hid_devices_num;

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_descriptor_length(void) {
    return sizeof(usb_hid_descriptor_template);
}

// Total length of the report descriptor, with all configured devices.
size_t usb_hid_report_descriptor_length(void) {
    return total_hid_report_descriptor_length;
}

bool usb_hid_enabled(void) {
    return usb_hid_is_enabled;
}

static const char usb_hid_interface_name[] =  USB_INTERFACE_NAME " HID";

// This is the interface descriptor, not the report descriptor.
size_t usb_hid_add_descriptor(uint8_t *descriptor_buf, uint8_t *current_interface, uint8_t *current_endpoint, uint8_t* current_interface_string, uint16_t report_descriptor_length) {
    memcpy(descriptor_buf, usb_hid_descriptor_template, sizeof(usb_hid_descriptor_template));

    descriptor_buf[HID_DESCRIPTOR_INTERFACE_INDEX] = *current_interface;
    (*current_interface)++;

    usb_add_interface_string(*current_interface, usb_hid_interface_name);
    descriptor_buf[HID_DESCRIPTOR_INTERFACE_STRING_INDEX] = *current_interface_string;
    (*current_interface_string)++;

    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX] = report_descriptor_length & 0xFF;
    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX + 1] = (report_descriptor_length >> 8);

    descriptor_buf[HID_IN_ENDPOINT_INDEX] = 0x80 | (USB_HID_EP_NUM_IN ? USB_HID_EP_NUM_IN : *current_endpoint);
    descriptor_buf[HID_OUT_ENDPOINT_INDEX] = USB_HID_EP_NUM_OUT ? USB_HID_EP_NUM_OUT : *current_endpoint;
    (*current_endpoint)++;

    return sizeof(usb_hid_descriptor_template);
}

static mp_rom_obj_tuple_t default_hid_devices_tuple = {
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

// Set the default list of devices that will be included. Called before boot.py runs, in the boot.py VM.
void common_hal_usb_hid_configure_usb_defaults(void) {
    common_hal_usb_hid_configure_usb(&default_hid_devices_tuple);
}

bool common_hal_usb_hid_configure_usb(mp_obj_t devices) {
    // We can't change the devices once we're connected.
    if (tud_connected()) {
        return false;
    }

    // Remember the devices for use in usb_hid_post_boot_py.
    hid_devices_seq = devices;
    return true;
}

// Called only once, before boot.py
void usb_hid_init_usb(void) {
    usb_hid_is_enabled = true;
}

// Build the combined HID report descriptor and save the chosen devices.
// Both are saved in supervisor allocations.
void usb_hid_post_boot_py(void) {

    // Build a combined report descriptor

    hid_devices_num = mp_obj_get_int(mp_obj_len(hid_devices_seq));

    // First get the total size.
    total_hid_report_descriptor_length = 0;
    for (mp_int_t i = 0; i < hid_devices_num; i++) {
        // hid_devices_seq has already been validated to contain only usb_hid_device_obj_t objects.
        usb_hid_device_obj_t *device =
            MP_OBJ_TO_PTR(mp_obj_subscr(hid_devices_seq, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));
        total_hid_report_descriptor_length += device->report_descriptor_length;
    }

    // Don't need space for a report id if there's only one device.
    if (hid_devices_num == 1) {
        total_hid_report_descriptor_length -= 2;
    }

    // Allocate storage that persists across VMs to build the combined report descriptor
    // and to remember the device details.

    hid_report_descriptor_allocation =
        allocate_memory(align32_size(total_hid_report_descriptor_length),
                        false /*highaddress*/, true /*movable*/);

    hid_devices_allocation =
        allocate_memory(align32_size(sizeof(usb_hid_device_obj_t) * hid_devices_num),
                        false /*highaddress*/, true /*movable*/);
    usb_hid_device_obj_t *hid_devices = (usb_hid_device_obj_t *) hid_devices_allocation->ptr;

    uint8_t *report_descriptor_start = (uint8_t *) hid_report_descriptor_allocation->ptr;

    for (mp_int_t i = 0; i < hid_devices_num; i++) {
        usb_hid_device_obj_t *device =
            MP_OBJ_TO_PTR(mp_obj_subscr(hid_devices_seq, MP_OBJ_NEW_SMALL_INT(i), MP_OBJ_SENTINEL));

        // Copy the report descriptor for this device.
        if (hid_devices_num == 1) {
            // Theres only one device, so it shouldn't have a report ID.
            // Copy the descriptor, but splice out the report id indicator and value (2 bytes).
            memcpy(report_descriptor_start, device->report_descriptor, device->report_id_index - 1);
            report_descriptor_start += device->report_id_index - 1;
            memcpy(report_descriptor_start, device->report_descriptor + device->report_id_index + 1,
                   device->report_descriptor_length - device->report_id_index - 1);
        } else {
            // Copy the whole descriptor and fill in the report id.
            memcpy(report_descriptor_start, device->report_descriptor, device->report_descriptor_length);
            report_descriptor_start[device->report_id_index] = i + 1;
            report_descriptor_start += device->report_descriptor_length;
        }

        // Copy the device data and discard any descriptor-bytes object pointer.
        memcpy(&hid_devices[i], device, sizeof(usb_hid_device_obj_t));
        hid_devices[i].report_descriptor_obj = mp_const_none;
    }

    // No longer keeping the Python object of devices to configure.
    hid_devices_seq = MP_OBJ_NULL;
}

void usb_hid_gc_collect(void) {
    if (tud_mounted()) {
        // Once tud_mounted() is true, we're done with the constructed descriptors.
        free_memory(hid_report_descriptor_allocation);
    }

    gc_collect_ptr(hid_devices_seq);
    gc_collect_ptr(hid_report_descriptor_allocation->ptr);
    gc_collect_ptr(hid_devices_allocation->ptr);
}

#if CIRCUITPY_USB_HID
// Invoked when GET HID REPORT DESCRIPTOR is received.
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
    return (uint8_t *) hid_report_descriptor_allocation->ptr;
}
#endif

usb_hid_device_obj_t *usb_hid_get_device_with_report_id(uint8_t report_id) {
    for (uint8_t i = 0; i < hid_devices_num; i++) {
        usb_hid_device_obj_t *hid_devices = (usb_hid_device_obj_t *) hid_devices_allocation->ptr;
        if (hid_devices[i].report_id == report_id) {
            return &hid_devices[i];
        }
    }
    return NULL;
}
