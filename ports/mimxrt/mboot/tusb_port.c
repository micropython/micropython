/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 * Copyright (c) 2021 Philipp Ebensberger
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

#include "tusb.h"
#include "mboot_utils.h"

/* A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
 * Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
 *
 * Auto ProductID layout's Bitmap:
 *   [MSB]         HID | MSC | CDC          [LSB]
 */
#define _PID_MAP(itf, n)  ((CFG_TUD_##itf) << (n))
#define USB_PID           (0x4000 | _PID_MAP(CDC, 0) | _PID_MAP(MSC, 1) | _PID_MAP(HID, 2) | \
    _PID_MAP(MIDI, 3) | _PID_MAP(VENDOR, 4))

#define ALT_COUNT 1
#define USBD_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + TUD_DFU_DESC_LEN(ALT_COUNT))
#define USBD_MAX_POWER_MA (100)
#define FUNC_ATTRS (DFU_ATTR_CAN_UPLOAD | DFU_ATTR_CAN_DOWNLOAD | DFU_ATTR_MANIFESTATION_TOLERANT)

enum {
    USBD_STR_0,
    USBD_STR_MANUF,
    USBD_STR_PRODUCT,
    USBD_STR_SERIAL,
    USBD_STR_DFU,
};

enum
{
    ITF_NUM_DFU_MODE,
    ITF_NUM_TOTAL
};

// --------------------------------------------------------------------+
// Device Descriptors
// --------------------------------------------------------------------+
static const tusb_desc_device_t usbd_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,

    #if CFG_TUD_CDC
    // Use Interface Association Descriptor (IAD) for CDC
    // As required by USB Specs IAD's subclass must be common class (2) and protocol must be IAD (1)
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    #else
    .bDeviceClass = 0x00,
    .bDeviceSubClass = 0x00,
    .bDeviceProtocol = 0x00,
    #endif

    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,

    .idVendor = 0xcafe,
    .idProduct = USB_PID,
    .bcdDevice = 0x0100,

    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,

    .bNumConfigurations = 0x01
};

// Invoked when received GET DEVICE DESCRIPTOR
const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&usbd_desc_device;
}

// --------------------------------------------------------------------+
// Configuration Descriptor
// --------------------------------------------------------------------+
static const uint8_t usbd_desc_cfg[USBD_DESC_TOTAL_LEN] = {
    // Config number, interface count, string index, total length, attribute, power in mA
    TUD_CONFIG_DESCRIPTOR(1, ITF_NUM_TOTAL, USBD_STR_0, USBD_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),
    // Interface number, string index, attributes, detach timeout, transfer size
    TUD_DFU_DESCRIPTOR(ITF_NUM_DFU_MODE, ALT_COUNT, USBD_STR_DFU, FUNC_ATTRS, 1000, CFG_TUD_DFU_XFER_BUFSIZE),
};

// Invoked when received GET CONFIGURATION DESCRIPTOR
const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return usbd_desc_cfg;
}

// --------------------------------------------------------------------+
// String Descriptors
// --------------------------------------------------------------------+

// array of pointer to string descriptors
static const char *const usbd_desc_str[] = {
    [USBD_STR_0] = (const char[]) { 0x09, 0x04 }, // Supported language is English (0x0409)
    [USBD_STR_MANUF] = "MicroPython",
    [USBD_STR_PRODUCT] = "Board in DFU mode",
    [USBD_STR_SERIAL] = "00000000000000000000",
    [USBD_STR_DFU] = "Board DFU",
};

// Invoked when received GET STRING DESCRIPTOR request
const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    #define DESC_STR_MAX (20)
    static uint16_t desc_str[DESC_STR_MAX];
    static const char hexchr[16] = "0123456789ABCDEF";

    memset(desc_str, 0, sizeof(desc_str));

    uint8_t len;
    if (index == 0) {
        desc_str[1] = 0x0409; // supported language is English
        len = 1;
    } else {
        if (index >= sizeof(usbd_desc_str) / sizeof(usbd_desc_str[0])) {
            return NULL;
        }
        if (index == USBD_STR_SERIAL) {
            uint8_t uid[8];
            mboot_utils_uid(uid);
            // store it as a hex string
            for (len = 0; len < 16; len += 2) {
                desc_str[1 + len] = hexchr[uid[len / 2] >> 4];
                desc_str[1 + len + 1] = hexchr[uid[len / 2] & 0x0f];
            }
        } else {
            const char *str = usbd_desc_str[index];
            for (len = 0; len < DESC_STR_MAX - 1 && str[len]; ++len) {
                desc_str[1 + len] = str[len];
            }
        }
    }

    // first byte is length (including header), second byte is string type
    desc_str[0] = (TUSB_DESC_STRING << 8) | (2 * len + 2);

    return desc_str;
}
