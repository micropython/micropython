/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Damien P. George
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
#ifndef MICROPY_INCLUDED_STMHAL_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H
#define MICROPY_INCLUDED_STMHAL_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H

// these are exports for the CDC/MSC/HID interface that are independent
// from any other definitions/declarations

// only CDC_MSC and CDC_HID are available
typedef enum {
    USBD_MODE_CDC = 0x01,
    USBD_MODE_MSC = 0x02,
    USBD_MODE_HID = 0x04,
    USBD_MODE_CDC_MSC = 0x03,
    USBD_MODE_CDC_HID = 0x05,
    USBD_MODE_MSC_HID = 0x06,
} usb_device_mode_t;

typedef struct _USBD_HID_ModeInfoTypeDef {
    uint8_t subclass; // 0=no sub class, 1=boot
    uint8_t protocol; // 0=none, 1=keyboard, 2=mouse
    uint8_t max_packet_len; // only support up to 255
    uint8_t polling_interval; // in units of 1ms
    uint8_t report_desc_len;
    const uint8_t *report_desc;
} USBD_HID_ModeInfoTypeDef;

#endif // MICROPY_INCLUDED_STMHAL_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H
