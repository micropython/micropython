/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
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
 *
 */

#ifndef MICROPY_INCLUDED_LIB_UTILS_TUSB_CONFIG_H
#define MICROPY_INCLUDED_LIB_UTILS_TUSB_CONFIG_H

#include <py/mpconfig.h>
#include "mpconfigport.h"

#if MICROPY_HW_ENABLE_USBDEV

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING "MicroPython"
#endif

#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING "Board in FS mode"
#endif

#ifndef MICROPY_HW_USB_CDC_INTERFACE_STRING
#define MICROPY_HW_USB_CDC_INTERFACE_STRING "Board CDC"
#endif

#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE)

#define CFG_TUD_CDC             (1)
#define CFG_TUD_CDC_EP_BUFSIZE  (256)
#define CFG_TUD_CDC_RX_BUFSIZE  (256)
#define CFG_TUD_CDC_TX_BUFSIZE  (256)

// MSC Configuration
#if CFG_TUD_MSC
#ifndef MICROPY_HW_USB_MSC_INTERFACE_STRING
#define MICROPY_HW_USB_MSC_INTERFACE_STRING "Board MSC"
#endif
#define CFG_TUD_MSC             (1)
// Set MSC EP buffer size to FatFS block size to avoid partial read/writes (offset arg).
#define CFG_TUD_MSC_BUFSIZE     (MICROPY_FATFS_MAX_SS)
#endif

// Define static descriptor size and interface count based on the above config

#if CFG_TUD_MSC
#define USBD_STATIC_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN + TUD_MSC_DESC_LEN)
#else
#define USBD_STATIC_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
#endif

#define USBD_STR_0 (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CDC (0x04)
#define USBD_STR_MSC (0x05)

#define USBD_MAX_POWER_MA (250)

#define USBD_DESC_STR_MAX (20)

#define USBD_ITF_CDC (0) // needs 2 interfaces

#define USBD_CDC_EP_CMD (0x81)
#define USBD_CDC_EP_OUT (0x02)
#define USBD_CDC_EP_IN (0x82)

#if CFG_TUD_MSC
#define USBD_ITF_MSC (2)
#define EPNUM_MSC_OUT    (0x03)
#define EPNUM_MSC_IN     (0x83)
#endif

/* Limits of statically defined USB interfaces, endpoints, strings */
#if CFG_TUD_MSC
#define USBD_ITF_STATIC_MAX (3)
#define USBD_STR_STATIC_MAX (USBD_STR_MSC + 1)
#define USBD_EP_STATIC_MAX (0x04) // ignoring the IN EP flag 0x80
#else
#define USBD_ITF_STATIC_MAX (2)
#define USBD_STR_STATIC_MAX (USBD_STR_CDC + 1)
#define USBD_EP_STATIC_MAX (0x03) // ignoring the IN EP flag 0x80
#endif

#endif // MICROPY_INCLUDED_LIB_UTILS_TUSB_CONFIG_H
