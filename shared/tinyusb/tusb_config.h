/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022 Angus Gratton
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

#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_TUSB_CONFIG_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_TUSB_CONFIG_H

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#ifndef MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE 0
#endif

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING "MicroPython"
#endif

#ifndef MICROPY_HW_USB_PRODUCT_FS_STRING
#define MICROPY_HW_USB_PRODUCT_FS_STRING "Board in FS mode"
#endif

#ifndef MICROPY_HW_USB_CDC_INTERFACE_STRING
#define MICROPY_HW_USB_CDC_INTERFACE_STRING "Board CDC"
#endif

#ifndef MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_VENDOR_STRING "MicroPy"
#endif

#ifndef MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_PRODUCT_STRING "Mass Storage"
#endif

#ifndef MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING
#define MICROPY_HW_USB_MSC_INQUIRY_REVISION_STRING "1.00"
#endif

#if !defined(CFG_TUSB_RHPORT0_MODE) && !defined(CFG_TUSB_RHPORT1_MODE)
#define CFG_TUSB_RHPORT0_MODE   (OPT_MODE_DEVICE)
#endif

#if MICROPY_HW_USB_CDC
#define CFG_TUD_CDC             (1)
#else
#define CFG_TUD_CDC             (0)
#endif

#if MICROPY_HW_USB_MSC
#define CFG_TUD_MSC             (1)
#else
#define CFG_TUD_MSC             (0)
#endif

// CDC Configuration
#if CFG_TUD_CDC
#ifndef CFG_TUD_CDC_RX_BUFSIZE
#define CFG_TUD_CDC_RX_BUFSIZE  ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 256)
#endif
#ifndef CFG_TUD_CDC_TX_BUFSIZE
#define CFG_TUD_CDC_TX_BUFSIZE  ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 256)
#endif
#endif // CFG_TUD_CDC

// MSC Configuration
#if CFG_TUD_MSC
#ifndef MICROPY_HW_USB_MSC_INTERFACE_STRING
#define MICROPY_HW_USB_MSC_INTERFACE_STRING "Board MSC"
#endif
// Set MSC EP buffer size to FatFS block size to avoid partial read/writes (offset arg).
#define CFG_TUD_MSC_BUFSIZE (MICROPY_FATFS_MAX_SS)
#endif // CFG_TUD_MSC

#if MICROPY_HW_NETWORK_USBNET
// TinyUSB supports both RNDIS and NCM protocols.
// NCM is now recommended by Microsoft on Win 11 and above.
#define CFG_TUD_NCM             (1)

// NCM TinyUSB buffer configuration.
// These depend on lwIP parameters (TCP_MSS, LWIP_MDNS_RESPONDER); lwipopts.h
// is safe to include here because MICROPY_HW_NETWORK_USBNET implies LWIP.
#include "lwipopts.h"

// Must be >> MTU; can be set to 2048 without impact.
#ifndef CFG_TUD_NCM_IN_NTB_MAX_SIZE
#define CFG_TUD_NCM_IN_NTB_MAX_SIZE  ((2 + LWIP_MDNS_RESPONDER) * TCP_MSS + 100)
#endif
#ifndef CFG_TUD_NCM_OUT_NTB_MAX_SIZE
#define CFG_TUD_NCM_OUT_NTB_MAX_SIZE ((2 + LWIP_MDNS_RESPONDER) * TCP_MSS + 100)
#endif

// Number of NCM transfer blocks for reception side.
#ifndef CFG_TUD_NCM_OUT_NTB_N
#define CFG_TUD_NCM_OUT_NTB_N 1
#endif
// Number of NCM transfer blocks for transmission side.
#ifndef CFG_TUD_NCM_IN_NTB_N
#define CFG_TUD_NCM_IN_NTB_N  (2 + LWIP_MDNS_RESPONDER)
#endif

#ifndef MICROPY_HW_NETWORK_USBNET_INTERFACE_STRING
#define MICROPY_HW_NETWORK_USBNET_INTERFACE_STRING "Board NET"
#endif
#endif // MICROPY_HW_NETWORK_USBNET

#define USBD_RHPORT (0) // Currently only one port is supported

// Define built-in interface, string and endpoint numbering based on the above config

enum _USBD_STR {
    USBD_STR_0 = (0x00),
    USBD_STR_MANUF = (0x01),
    USBD_STR_PRODUCT = (0x02),
    USBD_STR_SERIAL = (0x03),
    #if CFG_TUD_CDC
    USBD_STR_CDC,
    #endif
    #if CFG_TUD_MSC
    USBD_STR_MSC,
    #endif
    #if CFG_TUD_NCM
    USBD_STR_NET,
    USBD_STR_NET_MAC,
    #endif
    // One more than the highest built-in string index; runtime device string
    // indexes should start at or above this value to avoid collisions.
    USBD_STR_BUILTIN_MAX,
};

#define USBD_MAX_POWER_MA (250)

#ifndef MICROPY_HW_USB_DESC_STR_MAX
#define MICROPY_HW_USB_DESC_STR_MAX (40)
#endif

enum _USBD_ITF {
    #if CFG_TUD_CDC
    USBD_ITF_CDC,
    USBD_ITF_CDC_I2,
    #endif // CFG_TUD_CDC
    #if CFG_TUD_MSC
    USBD_ITF_MSC,
    #endif // CFG_TUD_MSC
    #if CFG_TUD_NCM
    USBD_ITF_NET,
    USBD_ITF_NET_DATA,
    #endif
};

enum _USBD_EP {
    USBD_EP_BASE = 0x80,
    #if CFG_TUD_CDC
    USBD_CDC_EP_CMD,
    USBD_CDC_EP_IN,
    #endif // CFG_TUD_CDC
    #if CFG_TUD_MSC
    USBD_MSC_EP_IN,
    #endif // CFG_TUD_MSC
    #if CFG_TUD_NCM
    USBD_NET_EP_CMD,
    USBD_NET_EP_IN,
    #endif // CFG_TUD_NCM
};

// define the matching in endpoints to each EP_OUT
#if CFG_TUD_CDC
#define USBD_CDC_EP_OUT  ((USBD_CDC_EP_IN)&~TUSB_DIR_IN_MASK)
#endif
#if CFG_TUD_MSC
#define USBD_MSC_EP_OUT  ((USBD_MSC_EP_IN)&~TUSB_DIR_IN_MASK)
#endif
#if CFG_TUD_NCM
#define USBD_NET_EP_OUT  ((USBD_NET_EP_IN)&~TUSB_DIR_IN_MASK)
#endif


/* Limits of builtin USB interfaces, endpoints, strings */
// Number of interfaces used by all enabled classes
#define USBD_ITF_BUILTIN_MAX ( \
    (CFG_TUD_CDC ? 2 : 0) + \
    (CFG_TUD_MSC ? 1 : 0) + \
    (CFG_TUD_NCM ? 2 : 0))

// 1 plus the number of interfaces used by all enabled classes
#define USBD_EP_BUILTIN_MAX ( \
    (CFG_TUD_CDC ? 2 : 0) + \
    (CFG_TUD_MSC ? 1 : 0) + \
    (CFG_TUD_NCM ? 2 : 0) + \
    1)

#endif // MICROPY_HW_ENABLE_USBDEV

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_TUSB_CONFIG_H
