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
#ifndef MICROPY_INCLUDED_STM32_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H
#define MICROPY_INCLUDED_STM32_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H

// these are exports for the CDC/MSC/HID interface that are independent
// from any other definitions/declarations

// These can be or'd together (but not all combinations may be available)
#define USBD_MODE_IFACE_MASK    (0x7f)
#define USBD_MODE_IFACE_CDC(i)  (0x01 << (i))
#define USBD_MODE_IFACE_HID     (0x10)
#define USBD_MODE_IFACE_MSC     (0x20)
#define USBD_MODE_HIGH_SPEED    (0x80)

// Convenience macros for supported mode combinations
#define USBD_MODE_CDC       (USBD_MODE_IFACE_CDC(0))
#define USBD_MODE_CDC2      (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_CDC(1))
#define USBD_MODE_CDC3      (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_CDC(1) | USBD_MODE_IFACE_CDC(2))
#define USBD_MODE_CDC_HID   (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_HID)
#define USBD_MODE_CDC_MSC   (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_MSC)
#define USBD_MODE_CDC2_MSC  (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_CDC(1) | USBD_MODE_IFACE_MSC)
#define USBD_MODE_CDC3_MSC  (USBD_MODE_IFACE_CDC(0) | USBD_MODE_IFACE_CDC(1) | USBD_MODE_IFACE_CDC(2) | USBD_MODE_IFACE_MSC)
#define USBD_MODE_HID       (USBD_MODE_IFACE_HID)
#define USBD_MODE_MSC       (USBD_MODE_IFACE_MSC)
#define USBD_MODE_MSC_HID   (USBD_MODE_IFACE_MSC | USBD_MODE_IFACE_HID)

typedef struct _USBD_HID_ModeInfoTypeDef {
    uint8_t subclass; // 0=no sub class, 1=boot
    uint8_t protocol; // 0=none, 1=keyboard, 2=mouse
    uint8_t max_packet_len; // only support up to 255
    uint8_t polling_interval; // in units of 1ms
    uint8_t report_desc_len;
    const uint8_t *report_desc;
} USBD_HID_ModeInfoTypeDef;

#endif // MICROPY_INCLUDED_STM32_USBDEV_CLASS_INC_USBD_CDC_MSC_HID0_H
