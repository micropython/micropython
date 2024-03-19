/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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

#include "tusb.h"
#include "mphalport.h"

#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID (0xf055)
#endif

#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID (0x9802)
#endif

#ifndef MICROPY_HW_USB_MANUFACTURER_STRING
#define MICROPY_HW_USB_MANUFACTURER_STRING ("MicroPython")
#endif

#define USBD_DESC_LEN (TUD_CONFIG_DESC_LEN + TUD_CDC_DESC_LEN)
#define USBD_MAX_POWER_MA (250)

#define USBD_ITF_CDC (0) // needs 2 interfaces
#define USBD_ITF_MAX (2)

#define USBD_CDC_EP_CMD (0x81)
#define USBD_CDC_EP_OUT (0x02)
#define USBD_CDC_EP_IN (0x82)
#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE (512)

#define USBD_STR_0 (0x00)
#define USBD_STR_MANUF (0x01)
#define USBD_STR_PRODUCT (0x02)
#define USBD_STR_SERIAL (0x03)
#define USBD_STR_CDC (0x04)

// Note: descriptors returned from callbacks must exist long enough for transfer to complete

static const tusb_desc_device_t usbd_desc_device = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = MICROPY_HW_USB_VID,
    .idProduct = MICROPY_HW_USB_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,
    .bNumConfigurations = 1,
};

static const uint8_t usbd_desc_cfg[USBD_DESC_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_MAX, USBD_STR_0, USBD_DESC_LEN,
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, USBD_MAX_POWER_MA),

    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
};

static const char *const usbd_desc_str[] = {
    [USBD_STR_MANUF] = MICROPY_HW_USB_MANUFACTURER_STRING,
    [USBD_STR_PRODUCT] = MICROPY_HW_BOARD_NAME,
    [USBD_STR_SERIAL] = "00000000000000000000",
    [USBD_STR_CDC] = "Board CDC",
};

const uint8_t *tud_descriptor_device_cb(void) {
    return (const uint8_t *)&usbd_desc_device;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return usbd_desc_cfg;
}

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
            mp_hal_get_unique_id(uid);
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
