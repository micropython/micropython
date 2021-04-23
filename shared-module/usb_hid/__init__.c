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

static const uint8_t usb_hid_descriptor_template[] = {
    0x09,        //  0 bLength
    0x21,        //  1 bDescriptorType (HID)
    0x11, 0x01,  //  2 bcdHID 1.11
    0x00,        //  3 bCountryCode
    0x01,        //  4 bNumDescriptors
    0x22,        //  5 bDescriptorType[0] (HID)
    0xFF, 0xFF,  //  6,7 wDescriptorLength[0]   [SET AT RUNTIME: lo, hi]
#define HID_DESCRIPTOR_LENGTH_INDEX (6)

    0x07,        //  8 bLength
    0x05,        //  9 bDescriptorType (Endpoint)
    0xFF,        // 10 bEndpointAddress (IN/D2H) [SET AT RUNTIME: 0x80 | endpoint]
#define HID_IN_ENDPOINT_INDEX (10)
    0x03,        // 11 bmAttributes (Interrupt)
    0x40, 0x00,  // 12,13  wMaxPacketSize 64
    0x08,        // 14 bInterval 8 (unit depends on device speed)

    0x07,        // 15 bLength
    0x05,        // 16 bDescriptorType (Endpoint)
    0xFF,        // 17 bEndpointAddress (OUT/H2D)  [SET AT RUNTIME]
#define HID_OUT_ENDPOINT_INDEX (17)
    0x03,        // 18 bmAttributes (Interrupt)
    0x40, 0x00,  // 19,20 wMaxPacketSize 64
    0x08,        // 21 bInterval 8 (unit depends on device speed)
};

// Is the HID device enabled?
bool usb_hid_enabled;
mp_obj_t usb_hid_devices;

size_t usb_hid_descriptor_length(void) {
    return sizeof(usb_hid_descriptor);
}

static const char[] usb_hid_interface_name =  USB_INTERFACE_NAME " Mass Storage";

size_t usb_hid_add_descriptor(uint8_t *descriptor_buf, uint8_t *current_interface, uint8_t *current_endpoint, uint8_t* current_interface_string, uint16_t report_descriptor_length) {
    memcpy(descriptor_buf, usb_hid_descriptor_template, sizeof(usb_hid_descriptor_template));

    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX] = report_descriptor_length & 0xFF;
    descriptor_buf[HID_DESCRIPTOR_LENGTH_INDEX + 1] = (report_descriptor_length >> 8);

    descriptor_buf[HID_IN_ENDPOINT_INDEX] = USB_HID_EP_NUM_IN ? USB_HID_EP_NUM_IN : *current_endpoint;
    descriptor_buf[HID_OUT_ENDPOINT_INDEX] = 0x80 | (USB_HID_EP_NUM_OUT ? USB_HID_EP_NUM_OUT : *current_endpoint);
    (*current_endpoint)++:

    return sizeof(usb_hid_descriptor_template);
}

void usb_hid_build_report_descriptor() {
}

bool common_hal_usb_hid_configure_usb(mp_obj_t devices) {
    // We can't change the devices once we're connected.
    if (tud_connected()) {
        return false;
    }

    // Assume no devices to start.
    usb_hid_enabled = false;
    if (devices == mp_const_none) {
        return true;
    }
}

void usb_hid_build_report

void usb_hid_gc_collect(void) {
    // Once tud_mounted() is true, we're done with the constructed descriptors.
    if (tud_mounted()) {
        // GC will pick up the inaccessible blocks.
        usb_hid_devices_to_configure = NULL;
    } else {
        gc_collect_ptr(usb_hid_devices);
    }
}
