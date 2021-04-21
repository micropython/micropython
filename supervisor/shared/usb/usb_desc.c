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

#include "lib/tinyusb/src/tusb.h"

#if CIRCUITPY_USB_CDC
#include "shared-module/storage/__init__.h"
#endif

#if CIRCUITPY_USB_HID
#include "shared-module/usb_hid/__init__.h"
#endif

#if CIRCUITPY_USB_MIDI
#include "shared-module/usb_midi/__init__.h"
#endif

#if CIRCUITPY_USB_MSC
#include "shared-module/storage/__init__.h"
#endif

#include "shared-module/usb_hid/Device.h"

#include "genhdr/autogen_usb_descriptor.h"

static uint8_t *device_descriptor;
static uint8_t *config_descriptor;

static const uint8_t device_descriptor_template[] = {
    0x12,        //  0 bLength
    0x01,        //  1 bDescriptorType (Device)
    0x00, 0x02,  //  2,3 bcdUSB 2.00
    0x00,        //  4 bDeviceClass (Use class information in the Interface Descriptors)
    0x00,        //  5 bDeviceSubClass
    0x00,        //  6 bDeviceProtocol
    0x40,        //  7 bMaxPacketSize0 64
    0x9A, 0x23,  //  8,9 idVendor [SET AT RUNTIME: lo,hi]
#define DEVICE_VID_LO_INDEX 8
#define DEVICE_VID_HI_INDEX 9
    0x, 0xFF,  // 10,11 idProduct [SET AT RUNTIME: lo,hi]
#define DEVICE PID_LO_INDEX 10
#define DEVICE PID_HI_INDEX 11
    0x00, 0x01,  // 12,13 bcdDevice 2.00
    0x02,        // 14 iManufacturer (String Index) [SET AT RUNTIME]
#define DEVICE_MANUFACTURER_STRING_INDEX 14
    0x03,        // 15 iProduct (String Index) [SET AT RUNTIME]
#define DEVICE_PRODUCT_STRING_INDEX 15
    0x01,        // 16 iSerialNumber (String Index)  [SET AT RUNTIME]
#define DEVICE_SERIAL_NUMBER_STRING_INDEX 16
    0x01,        // 17 bNumConfigurations 1
};

static const uint8_t configuration_descriptor_template[] = {
    0x09,        // 0 bLength
    0x02,        // 1 bDescriptorType (Configuration)
    0xFF, 0xFF,  // 2,3 wTotalLength  [SET AT RUNTIME: lo, hi]
#define CONFIG_TOTAL_LENGTH_LO_INDEX 2
#define CONFIG_TOTAL_LENGTH_HI_INDEX 3
    0xFF,        // 4 bNumInterfaces  [SET AT RUNTIME]
#define CONFIG_NUM_INTERFACES_INDEX 4
    0x01,        // 5 bConfigurationValue
    0x00,        // 6 iConfiguration (String Index)
    0x80,        // 7 bmAttributes
    0x32,        // 8 bMaxPower 100mA
};

void build_usb_device_descriptor(uint16_t vid, uint16_t pid, uint8_t manufacturer_string_index, uint8_t product_string_index, uint8_t serial_number_string_index) {

    device_descriptor = m_malloc(sizeof(device_descriptor_template), false);
    memcpy(device_descriptor, device_descriptor_template, sizeof(device_descriptor_template));

    device_descriptor[DEVICE_VID_LO_INDEX] = vid & 0xFF;
    device_descriptor[DEVICE_VID_HI_INDEX] = vid >> 8;
    device_descriptor[DEVICE_PID_LO_INDEX] = pid & 0xFF;
    device_descriptor[DEVICE_PID_HI_INDEX] = pid >> 8;
    device_descriptor[DEVICE_MANUFACTURER_STRING_INDEX] = manufacturer_string_index;
    device_descriptor[DEVICE_PRODUCT_STRING_INDEX] = product_string_index;
    device_descriptor[DEVICE_SERIAL_NUMBER_STRING_INDEX] = serial_number_string_index;
}

void build_usb_configuration_descriptor(uint16_t total_length, uint8_t num_interfaces) {
    size_t total_descriptor_length = sizeof(configuration_descriptor_template);

    // CDC should be first, for compatibility with Adafruit Windows 7 drivers.
    // In the past, the order has been CDC, MSC, MIDI, HID, so preserve
    // that order.
#if CIRCUITPY_USB_CDC
    if (usb_cdc_repl_enabled) {
        total_descriptor_length += usb_cdc_descriptor_length();
    }
    if (usb_cdc_data_enabled) {
        total_descriptor_length += usb_cdc_descriptor_length();
    }
#endif

#if CIRCUITPY_USB_MSC
    if (storage_usb_enabled) {
        total_descriptor_length += storage_usb_descriptor_length();
    }
#endif

#if CIRCUITPY_USB_MIDI
    if (usb_midi_enabled) {
        total_descriptor_length += usb_midi_descriptor_length();
    }
#endif

#if CIRCUITPY_USB_HID
    if (usb_hid_enabled) {
        total_descriptor_length += usb_hid_descriptor_length();
    }
#endif

    // Now we now how big the configuration descriptor will be.
    configuration_descriptor = m_malloc(total_descriptor_length, false);

    // Copy the top-level template, and fix up its length.
    memcpy(config_descriptor, configuration_descriptor_template, sizeof(configuration_descriptor_template));
    configuration_descriptor[CONFIG_TOTAL_LENGTH_LO_INDEX] = total_descriptor_length & 0xFF;
    configuration_descriptor[CONFIG_TOTAL_LENGTH_HI_INDEX] = (total_descriptor_length >> 8) & 0xFF;

    // Number interfaces and endpoints.
    // Endpoint 0 is already used for USB control.
    uint8_t current_interface = 0;
    uint8_t current_endpoint = 1;
    uint16_t current_interface_string = 1;

    uint8_t *descriptor_buf_remaining = configuration_descriptor + sizeof(configuration_descriptor_template);

#if CIRCUITPY_USB_CDC
    if (usb_cdc_repl_enabled) {
        // Concatenate and fix up the CDC REPL descriptor.
    }
    if (usb_cdc_data_enabled) {
        // Concatenate and fix up the CDC data descriptor.
    }
#endif

#if CIRCUITPY_USB_MSC
    if (storage_usb_enabled) {
        // Concatenate and fix up the MSC descriptor.
        descriptor_buf_remaining += storage_usb_add_descriptor(
            descriptor_buf_remaining, current_interface,
            current_endpoint,  // in
            current_endpoint,  // out
            current_interface_string_index);
        current_interface++;
        current_endpoint++;
        current_interface_string++;
    }
#endif

#if CIRCUITPY_USB_MIDI
    if (usb_midi_enabled) {
        // Concatenate and fix up the MIDI descriptor.
        descriptor_buf_remaining += usb_midi_add_descriptor(
            descriptor_buf_remaining,
            current_interface,            // audio control
            current_interface + 1,        // MIDI streaming
            current_endpoint,             // in
            current_endpoint,             // out
            current_interface_string,     // audio control
            current_interface_string + 1  // MIDI streaming
            current_interface_string + 2  // in jack
            current_interface_string + 3  // out jack
            );
        current_interface += 2;           // two interfaces: audio control and MIDI streaming
        current_endpoint++;               // MIDI streaming only (no audio data)
        current_interface_string += 4;    // two interface names: audio control and MIDI streaming

    }
#endif

#if CIRCUITPY_USB_HID
    if (usb_hid_enabled) {
        // Concatenate and fix up the HID descriptor (not the report descriptors).
    }
#endif

    // Now we know how many interfaces have been used.
    configuration_descriptor[CONFIG_NUM_INTERFACES_INDEX] = current_interface - 1;

    // Did we run out of endpoints?
    if (current_endpoint - 1 > USB_NUM_EP) {
        mp_raise_SystemError("Not enough USB endpoints");
    }

}

// Invoked when received GET DEVICE DESCRIPTOR
// Application return pointer to descriptor
uint8_t const *tud_descriptor_device_cb(void) {
    return usb_descriptor_dev;
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;  // for multiple configurations
    return config_desc;
}

#if CIRCUITPY_USB_HID
// Invoked when received GET HID REPORT DESCRIPTOR
// Application return pointer to descriptor
// Descriptor contents must exist long enough for transfer to complete
uint8_t const *tud_hid_descriptor_report_cb(uint8_t itf) {
    (void)itf;
    return hid_report_descriptor;
}
#endif

// Invoked when received GET STRING DESCRIPTOR request
// Application return pointer to descriptor, whose contents must exist long enough for transfer to complete
uint16_t const *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    uint8_t const max_index = sizeof(string_descriptor_arr) / sizeof(string_descriptor_arr[0]);
    return (index < max_index) ? string_descriptor_arr[index] : NULL;
}
