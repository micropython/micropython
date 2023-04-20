/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
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
 */

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#include "tusb.h"
#include "mp_usbd.h"
#include "mp_usbd_internal.h"

#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE (64)

const tusb_desc_device_t mp_usbd_desc_device_static = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    #ifdef MICROPY_HW_USB_VENDOR
    .bcdUSB = 0x0210,
    #else
    .bcdUSB = 0x0200,
    #endif
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

const uint8_t mp_usbd_desc_cfg_static[USBD_STATIC_DESC_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_STATIC_MAX, USBD_STR_0, USBD_STATIC_DESC_LEN,
        0, USBD_MAX_POWER_MA),

    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, 5, EPNUM_MSC_OUT, EPNUM_MSC_IN, 64),
    #endif
    #if CFG_TUD_VENDOR
    TUD_VENDOR_DESCRIPTOR(USBD_ITF_VENDOR, USBD_STR_VENDOR, EPNUM_VENDOR_OUT, EPNUM_VENDOR_IN, 64),
    #endif

};

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    char serial_buf[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes terminating NUL byte
    static uint16_t desc_wstr[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes prefix uint16_t
    const char *desc_str;
    uint16_t desc_len;

    switch (index) {
        case 0:
            desc_wstr[1] = 0x0409; // supported language is English
            desc_len = 4;
            break;
        case USBD_STR_SERIAL:
            // TODO: make a port-specific serial number callback
            mp_usbd_port_get_serial_number(serial_buf);
            desc_str = serial_buf;
            break;
        case USBD_STR_MANUF:
            desc_str = MICROPY_HW_USB_MANUFACTURER_STRING;
            break;
        case USBD_STR_PRODUCT:
            desc_str = MICROPY_HW_USB_PRODUCT_FS_STRING;
            break;
        #if CFG_TUD_CDC
        case USBD_STR_CDC:
            desc_str = MICROPY_HW_USB_CDC_INTERFACE_STRING;
            break;
        #endif
        #if CFG_TUD_MSC
        case USBD_STR_MSC:
            desc_str = MICROPY_HW_USB_MSC_INTERFACE_STRING;
            break;
        #endif
        #if CFG_TUD_VENDOR
        case USBD_STR_VENDOR:
            desc_str = MICROPY_HW_USB_VENDOR_INTERFACE_STRING;
            break;
        #endif
        default:
            desc_str = NULL;
    }

    if (index != 0) {
        if (desc_str == NULL) {
            return NULL; // Will STALL the endpoint
        }

        // Convert from narrow string to wide string
        desc_len = 2;
        for (int i = 0; i < MICROPY_HW_USB_DESC_STR_MAX && desc_str[i] != 0; i++) {
            desc_wstr[1 + i] = desc_str[i];
            desc_len += 2;
        }
    }
    // first byte is length (including header), second byte is string type
    desc_wstr[0] = (TUSB_DESC_STRING << 8) | desc_len;

    return desc_wstr;
}


const uint8_t *tud_descriptor_device_cb(void) {
    return (const void *)&mp_usbd_desc_device_static;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    return mp_usbd_desc_cfg_static;
}

#if CFG_TUD_VENDOR

// --------------------------------------------------------------------+
// BOS Descriptor
// --------------------------------------------------------------------+

/* Microsoft OS 2.0 registry property descriptor
Per MS requirements https://msdn.microsoft.com/en-us/library/windows/hardware/hh450799(v=vs.85).aspx
device should create DeviceInterfaceGUIDs. It can be done by driver and
in case of real PnP solution device should expose MS "Microsoft OS 2.0
registry property descriptor". Such descriptor can insert any record
into Windows registry per device/configuration/interface. In our case it
will insert "DeviceInterfaceGUIDs" multistring property.


https://developers.google.com/web/fundamentals/native-hardware/build-for-webusb/
(Section Microsoft OS compatibility descriptors)
*/
#define MS_OS_20_DESC_LEN  0xB2

#define BOS_TOTAL_LEN      (TUD_BOS_DESC_LEN + TUD_BOS_MICROSOFT_OS_DESC_LEN)

uint8_t const desc_bos[] =
{
    // total length, number of device caps
    TUD_BOS_DESCRIPTOR(BOS_TOTAL_LEN, 1),

    // Microsoft OS 2.0 descriptor
    TUD_BOS_MS_OS_20_DESCRIPTOR(MS_OS_20_DESC_LEN, 1)
};

uint8_t const desc_ms_os_20[] =
{
    // Set header: length, type, windows version, total length
    U16_TO_U8S_LE(0x000A), U16_TO_U8S_LE(MS_OS_20_SET_HEADER_DESCRIPTOR), U32_TO_U8S_LE(0x06030000), U16_TO_U8S_LE(MS_OS_20_DESC_LEN),

    // Configuration subset header: length, type, configuration index, reserved, configuration total length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_CONFIGURATION), 0, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A),

    // Function Subset header: length, type, first interface, reserved, subset length
    U16_TO_U8S_LE(0x0008), U16_TO_U8S_LE(MS_OS_20_SUBSET_HEADER_FUNCTION), USBD_ITF_VENDOR, 0, U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08),

    // MS OS 2.0 Compatible ID descriptor: length, type, compatible ID, sub compatible ID
    U16_TO_U8S_LE(0x0014), U16_TO_U8S_LE(MS_OS_20_FEATURE_COMPATBLE_ID), 'W', 'I', 'N', 'U', 'S', 'B', 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // sub-compatible

    // MS OS 2.0 Registry property descriptor: length, type
    U16_TO_U8S_LE(MS_OS_20_DESC_LEN - 0x0A - 0x08 - 0x08 - 0x14), U16_TO_U8S_LE(MS_OS_20_FEATURE_REG_PROPERTY),
    U16_TO_U8S_LE(0x0007), U16_TO_U8S_LE(0x002A), // wPropertyDataType, wPropertyNameLength and PropertyName "DeviceInterfaceGUIDs\0" in UTF-16
    'D', 0x00, 'e', 0x00, 'v', 0x00, 'i', 0x00, 'c', 0x00, 'e', 0x00, 'I', 0x00, 'n', 0x00, 't', 0x00, 'e', 0x00,
    'r', 0x00, 'f', 0x00, 'a', 0x00, 'c', 0x00, 'e', 0x00, 'G', 0x00, 'U', 0x00, 'I', 0x00, 'D', 0x00, 's', 0x00, 0x00, 0x00,
    U16_TO_U8S_LE(0x0050), // wPropertyDataLength
    // bPropertyData "{CDB3B5AD-293B-4663-AA36-1AAE46463776}" as a UTF-16 string (b doesn't mean bytes)
    '{', 0x00, 'C', 0x00, 'D', 0x00, 'B', 0x00, '3', 0x00, 'B', 0x00, '5', 0x00, 'A', 0x00, 'D', 0x00, '-', 0x00,
    '2', 0x00, '9', 0x00, '3', 0x00, 'B', 0x00, '-', 0x00, '4', 0x00, '6', 0x00, '6', 0x00, '3', 0x00, '-', 0x00,
    'A', 0x00, 'A', 0x00, '3', 0x00, '6', 0x00, '-', 0x00, '1', 0x00, 'A', 0x00, 'A', 0x00, 'E', 0x00, '4', 0x00,
    '6', 0x00, '4', 0x00, '6', 0x00, '3', 0x00, '7', 0x00, '7', 0x00, '6', 0x00, '}', 0x00, 0x00, 0x00, 0x00, 0x00
};

TU_VERIFY_STATIC(sizeof(desc_ms_os_20) == MS_OS_20_DESC_LEN, "Incorrect size");

uint8_t const *tud_descriptor_bos_cb(void) {
    return desc_bos;
}


extern uint8_t const desc_ms_os_20[];

bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage,
    tusb_control_request_t const *request) {
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) {
        return true;
    }

    switch (request->bmRequestType_bit.type)
    {
        case TUSB_REQ_TYPE_VENDOR:
            switch (request->bRequest)
            {
                case 1:
                    if (request->wIndex == 7) {
                        // Get Microsoft OS 2.0 compatible descriptor
                        uint16_t total_len;
                        memcpy(&total_len, desc_ms_os_20 + 8, 2);

                        return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, total_len);
                    } else {
                        return false;
                    }

                default:
                    break;
            }
            break;
        default:
            break;
    }

    // stall unknown request
    return false;
}
#endif // CFG_TUD_VENDOR

#endif
