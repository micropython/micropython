#ifndef MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
#define MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H

#include <stdint.h>

const uint8_t usb_desc_dev[18];
// Make sure the control buffer is big enough to fit the descriptor.
#define CFG_TUD_ENUM_BUFFER_SIZE 211
const uint8_t usb_desc_cfg[211];
uint16_t usb_serial_number[17];
uint16_t const * const string_desc_arr [12];

const uint8_t hid_report_descriptor[195];

// Vendor name included in Inquiry response, max 8 bytes
#define CFG_TUD_MSC_VENDOR          "Adafruit"

// Product name included in Inquiry response, max 16 bytes
#define CFG_TUD_MSC_PRODUCT         "A glorious potat"

#define USB_HID_REPORT_ID_KEYBOARD 1
#define USB_HID_REPORT_ID_GAMEPAD 5
#define USB_HID_REPORT_ID_MOUSE 2
#define USB_HID_REPORT_ID_CONSUMER 3

#define USB_HID_REPORT_LENGTH_KEYBOARD 8
#define USB_HID_REPORT_LENGTH_GAMEPAD 6
#define USB_HID_REPORT_LENGTH_MOUSE 4
#define USB_HID_REPORT_LENGTH_CONSUMER 2

#define USB_HID_NUM_DEVICES 4
#define USB_HID_MAX_REPORT_LENGTH 8
#endif // MICROPY_INCLUDED_AUTOGEN_USB_DESCRIPTOR_H
