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


#define USBD_CDC_CMD_MAX_SIZE (8)
#define USBD_CDC_IN_OUT_MAX_SIZE ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 64)
#define USBD_MSC_IN_OUT_MAX_SIZE ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 64)

const tusb_desc_device_t mp_usbd_builtin_desc_dev = {
    .bLength = sizeof(tusb_desc_device_t),
    .bDescriptorType = TUSB_DESC_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .idVendor = MICROPY_HW_USB_RUNTIME_VID,
    .idProduct = MICROPY_HW_USB_RUNTIME_PID,
    .bcdDevice = 0x0100,
    .iManufacturer = USBD_STR_MANUF,
    .iProduct = USBD_STR_PRODUCT,
    .iSerialNumber = USBD_STR_SERIAL,
    .bNumConfigurations = 1,
};


// Global class enable state
mp_usbd_class_state_t mp_usbd_class_state;

// CDC descriptor template with interface number placeholders (0xFF)
// Expands TUD_CDC_DESCRIPTOR macro with 0xFF for interface numbers
#if CFG_TUD_CDC
static const uint8_t cdc_desc_template[] = {
    // Interface Associate
    8, TUSB_DESC_INTERFACE_ASSOCIATION, 0xFF, 2, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, 0,
    // CDC Control Interface
    9, TUSB_DESC_INTERFACE, 0xFF, 0, 1, TUSB_CLASS_CDC, CDC_COMM_SUBCLASS_ABSTRACT_CONTROL_MODEL, CDC_COMM_PROTOCOL_NONE, USBD_STR_CDC,
    // CDC Header
    5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_HEADER, U16_TO_U8S_LE(0x0120),
    // CDC Call
    5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_CALL_MANAGEMENT, 0, 0xFE, // 0xFE = itf_num + 1
    // CDC ACM
    4, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_ABSTRACT_CONTROL_MANAGEMENT, 6,
    // CDC Union
    5, TUSB_DESC_CS_INTERFACE, CDC_FUNC_DESC_UNION, 0xFF, 0xFE, // 0xFF = itf_num, 0xFE = itf_num + 1
    // Endpoint Notification
    7, TUSB_DESC_ENDPOINT, USBD_CDC_EP_CMD, TUSB_XFER_INTERRUPT, U16_TO_U8S_LE(USBD_CDC_CMD_MAX_SIZE), 16,
    // CDC Data Interface
    9, TUSB_DESC_INTERFACE, 0xFE, 0, 2, TUSB_CLASS_CDC_DATA, 0, 0, 0, // 0xFE = itf_num + 1
    // Endpoint Out
    7, TUSB_DESC_ENDPOINT, USBD_CDC_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(USBD_CDC_IN_OUT_MAX_SIZE), 0,
    // Endpoint In
    7, TUSB_DESC_ENDPOINT, USBD_CDC_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(USBD_CDC_IN_OUT_MAX_SIZE), 0
};
#endif

// MSC descriptor template with interface number placeholders (0xFF)
// Expands TUD_MSC_DESCRIPTOR macro with 0xFF for interface number
#if CFG_TUD_MSC
static const uint8_t msc_desc_template[] = {
    // Interface
    9, TUSB_DESC_INTERFACE, 0xFF, 0, 2, TUSB_CLASS_MSC, MSC_SUBCLASS_SCSI, MSC_PROTOCOL_BOT, USBD_STR_MSC,
    // Endpoint Out
    7, TUSB_DESC_ENDPOINT, USBD_MSC_EP_OUT, TUSB_XFER_BULK, U16_TO_U8S_LE(USBD_MSC_IN_OUT_MAX_SIZE), 0,
    // Endpoint In
    7, TUSB_DESC_ENDPOINT, USBD_MSC_EP_IN, TUSB_XFER_BULK, U16_TO_U8S_LE(USBD_MSC_IN_OUT_MAX_SIZE), 0
};
#endif

// Lookup table for all USB class descriptors (extern for inlining in header)
const usb_desc_entry_t usb_desc_table[] = {
    #if CFG_TUD_CDC
    {USB_BUILTIN_FLAG_CDC, TUD_CDC_DESC_LEN, 2, cdc_desc_template},
    #endif
    #if CFG_TUD_MSC
    {USB_BUILTIN_FLAG_MSC, TUD_MSC_DESC_LEN, 1, msc_desc_template},
    #endif
};

// Global descriptor buffer for runtime configuration (used by both runtime and static modes)
uint8_t mp_usbd_desc_cfg_buffer[MP_USBD_BUILTIN_DESC_CFG_LEN];

// Unified descriptor generation function using lookup table
const uint8_t *mp_usbd_generate_desc_cfg_unified(uint8_t flags, uint8_t *buffer) {
    uint8_t *desc = buffer;
    usb_desc_info_t info = mp_usbd_get_desc_info_from_flags(flags);
    uint8_t interface_count = info.interface_count;
    size_t total_len = info.length;

    // Configuration descriptor header (unrolled for size optimization)
    *desc++ = 9;                           // bLength
    *desc++ = TUSB_DESC_CONFIGURATION;     // bDescriptorType
    *desc++ = (total_len) & 0xFF;          // wTotalLength low
    *desc++ = (total_len >> 8) & 0xFF;     // wTotalLength high
    *desc++ = interface_count;             // bNumInterfaces
    *desc++ = 1;                           // bConfigurationValue
    *desc++ = USBD_STR_0;                  // iConfiguration
    *desc++ = 0x80;                        // bmAttributes (bit 7 must be 1)
    *desc++ = USBD_MAX_POWER_MA / 2;       // bMaxPower (in 2mA units)

    // Track current interface number for dynamic assignment
    uint8_t itf_num = 0;

    // Iterate through lookup table and copy enabled class descriptors
    for (size_t i = 0; i < MP_ARRAY_SIZE(usb_desc_table); i++) {
        if (flags & usb_desc_table[i].flag_mask) {
            const uint8_t *src = usb_desc_table[i].template;
            uint8_t len = usb_desc_table[i].desc_len;

            // Copy descriptor with interface number substitution
            // 0xFF = itf_num, 0xFE = itf_num + 1
            for (uint8_t j = 0; j < len; j++) {
                uint8_t byte = src[j];
                if (byte == 0xFF) {
                    *desc++ = itf_num;
                } else if (byte == 0xFE) {
                    *desc++ = itf_num + 1;
                } else {
                    *desc++ = byte;
                }
            }

            itf_num += usb_desc_table[i].itf_count;
        }
    }

    return buffer;
}

#if !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE


// Get dynamic descriptor length based on enabled classes
size_t mp_usbd_get_descriptor_cfg_len(void) {
    size_t len = TUD_CONFIG_DESC_LEN;

    if (mp_usbd_class_state.cdc_enabled && CFG_TUD_CDC) {
        len += TUD_CDC_DESC_LEN;
    }
    if (mp_usbd_class_state.msc_enabled && CFG_TUD_MSC) {
        len += TUD_MSC_DESC_LEN;
    }

    return len;
}


#if 0
// Static descriptor for maximum possible configuration
static const uint8_t mp_usbd_builtin_desc_cfg_max[MP_USBD_BUILTIN_DESC_CFG_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_BUILTIN_MAX, USBD_STR_0, MP_USBD_BUILTIN_DESC_CFG_LEN,
        0, USBD_MAX_POWER_MA),

    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_EP_OUT, USBD_MSC_EP_IN, USBD_MSC_IN_OUT_MAX_SIZE),
    #endif
};
#endif


// Dynamic descriptor buffer for runtime configuration
static uint8_t mp_usbd_dynamic_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN];

// Convert class state to flags
static uint8_t mp_usbd_class_state_to_flags(void) {
    uint8_t flags = USB_BUILTIN_FLAG_NONE;
    if (mp_usbd_class_state.cdc_enabled) {
        flags |= USB_BUILTIN_FLAG_CDC;
    }
    if (mp_usbd_class_state.msc_enabled) {
        flags |= USB_BUILTIN_FLAG_MSC;
    }
    return flags;
}

// Generate dynamic configuration descriptor based on enabled classes
static const uint8_t *mp_usbd_generate_desc_cfg(void) {
    uint8_t flags = mp_usbd_class_state_to_flags();
    return mp_usbd_generate_desc_cfg_unified(flags, mp_usbd_dynamic_desc_cfg);
}
#endif

// Static fallback descriptor (needed for boot before Python initializes in runtime mode)
const uint8_t mp_usbd_builtin_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_BUILTIN_MAX, USBD_STR_0, MP_USBD_BUILTIN_DESC_CFG_LEN,
        0, USBD_MAX_POWER_MA),

    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, EPNUM_MSC_OUT, EPNUM_MSC_IN, USBD_MSC_IN_OUT_MAX_SIZE),
    #endif
};

const uint16_t *tud_descriptor_string_cb(uint8_t index, uint16_t langid) {
    char serial_buf[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes terminating NUL byte
    static uint16_t desc_wstr[MICROPY_HW_USB_DESC_STR_MAX + 1]; // Includes prefix uint16_t
    const char *desc_str = NULL;
    uint16_t desc_len;

    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    desc_str = mp_usbd_runtime_string_cb(index);
    #endif

    if (index == 0) {
        // String descriptor 0 is special, see USB 2.0 section 9.6.7 String
        //
        // Expect any runtime value in desc_str to be a fully formed descriptor
        if (desc_str == NULL) {
            desc_str = "\x04\x03\x09\x04"; // Descriptor for "English"
        }
        if (desc_str[0] < sizeof(desc_wstr)) {
            memcpy(desc_wstr, desc_str, desc_str[0]);
            return desc_wstr;
        }
        return NULL; // Descriptor length too long (or malformed), stall endpoint
    }

    // Otherwise, generate a "UNICODE" string descriptor from the C string

    if (desc_str == NULL) {
        // Fall back to the "static" string
        switch (index) {
            case USBD_STR_SERIAL:
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
            default:
                break;
        }
    }

    if (desc_str == NULL) {
        return NULL; // No string, STALL the endpoint
    }

    // Convert from narrow string to wide string
    desc_len = 2;
    for (int i = 0; i < MICROPY_HW_USB_DESC_STR_MAX && desc_str[i] != 0; i++) {
        desc_wstr[1 + i] = desc_str[i];
        desc_len += 2;
    }

    // first byte is length (including header), second byte is string type
    desc_wstr[0] = (TUSB_DESC_STRING << 8) | desc_len;

    return desc_wstr;
}


#if !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

const uint8_t *tud_descriptor_device_cb(void) {
    return (const void *)&mp_usbd_builtin_desc_dev;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    // Use dynamic generation to include only enabled classes
    return mp_usbd_generate_desc_cfg();
}

#else

// If runtime device support is enabled, descriptor callbacks are implemented in usbd.c

#endif // !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

#endif // MICROPY_HW_ENABLE_USBDEV
