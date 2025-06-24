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
#define USBD_NET_IN_OUT_MAX_SIZE ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 64)

const tusb_desc_device_t mp_usbd_builtin_desc_dev = {
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

// Global class enable state
mp_usbd_class_state_t mp_usbd_class_state;

// Initialize class state based on compile-time configuration and runtime mode
void mp_usbd_init_class_state(void) {
    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    // In runtime mode, only CDC enabled by default
    mp_usbd_class_state.cdc_enabled = (CFG_TUD_CDC == 1);
    mp_usbd_class_state.msc_enabled = false;
    mp_usbd_class_state.ncm_enabled = false;
    #else
    // In static mode, enable all compiled classes
    mp_usbd_class_state.cdc_enabled = (CFG_TUD_CDC == 1);
    mp_usbd_class_state.msc_enabled = (CFG_TUD_MSC == 1);
    mp_usbd_class_state.ncm_enabled = (CFG_TUD_NCM == 1);
    #endif
}

// Update class state based on bitfield flags
void mp_usbd_update_class_state(uint8_t flags) {
    #if MICROPY_HW_NETWORK_USBNET
    bool prev_ncm_enabled = mp_usbd_class_state.ncm_enabled;
    #endif

    mp_usbd_class_state.cdc_enabled = (flags & 0x01) && (CFG_TUD_CDC == 1);  // USB_BUILTIN_FLAG_CDC
    mp_usbd_class_state.msc_enabled = (flags & 0x02) && (CFG_TUD_MSC == 1);  // USB_BUILTIN_FLAG_MSC
    mp_usbd_class_state.ncm_enabled = (flags & 0x04) && (CFG_TUD_NCM == 1);  // USB_BUILTIN_FLAG_NCM

    // If NCM is being disabled and it was active, clean it up
    #if MICROPY_HW_NETWORK_USBNET
    if (prev_ncm_enabled && !mp_usbd_class_state.ncm_enabled) {
        extern void usbnet_deinit(void);
        usbnet_deinit();
    }
    #endif
}

// Functions to generate descriptors from flags (for new bitfield-based system)
size_t mp_usbd_get_descriptor_cfg_len_from_flags(uint8_t flags) {
    size_t len = TUD_CONFIG_DESC_LEN;

    if ((flags & 0x01) && CFG_TUD_CDC) {  // USB_BUILTIN_FLAG_CDC
        len += TUD_CDC_DESC_LEN;
    }
    if ((flags & 0x02) && CFG_TUD_MSC) {  // USB_BUILTIN_FLAG_MSC
        len += TUD_MSC_DESC_LEN;
    }
    if ((flags & 0x04) && CFG_TUD_NCM) {  // USB_BUILTIN_FLAG_NCM
        len += TUD_CDC_NCM_DESC_LEN;
    }

    return len;
}

static uint8_t mp_usbd_get_interface_count_from_flags(uint8_t flags) {
    uint8_t count = 0;

    if ((flags & 0x01) && CFG_TUD_CDC) {  // USB_BUILTIN_FLAG_CDC
        count += 2;  // CDC uses 2 interfaces
    }
    if ((flags & 0x02) && CFG_TUD_MSC) {  // USB_BUILTIN_FLAG_MSC
        count += 1;
    }
    if ((flags & 0x04) && CFG_TUD_NCM) {  // USB_BUILTIN_FLAG_NCM
        count += 1;
    }

    return count;
}

const uint8_t *mp_usbd_generate_desc_cfg_from_flags(uint8_t flags) {
    static uint8_t desc_cfg_buffer[MP_USBD_BUILTIN_DESC_CFG_LEN];
    uint8_t *desc = desc_cfg_buffer;
    uint8_t interface_count = mp_usbd_get_interface_count_from_flags(flags);
    size_t total_len = mp_usbd_get_descriptor_cfg_len_from_flags(flags);

    // Configuration descriptor header
    *desc++ = 9;                           // bLength
    *desc++ = TUSB_DESC_CONFIGURATION;     // bDescriptorType
    *desc++ = (total_len) & 0xFF;          // wTotalLength low
    *desc++ = (total_len >> 8) & 0xFF;     // wTotalLength high
    *desc++ = interface_count;             // bNumInterfaces
    *desc++ = 1;                           // bConfigurationValue
    *desc++ = USBD_STR_0;                  // iConfiguration
    *desc++ = 0;                           // bmAttributes
    *desc++ = USBD_MAX_POWER_MA / 2;       // bMaxPower (in 2mA units)

    // Add enabled class descriptors
    #if CFG_TUD_CDC
    if (flags & 0x01) {  // USB_BUILTIN_FLAG_CDC
        const uint8_t cdc_desc[] = {
            TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
                USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE)
        };
        memcpy(desc, cdc_desc, sizeof(cdc_desc));
        desc += sizeof(cdc_desc);
    }
    #endif

    #if CFG_TUD_MSC
    if (flags & 0x02) {  // USB_BUILTIN_FLAG_MSC
        const uint8_t msc_desc[] = {
            TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_EP_OUT, USBD_MSC_EP_IN, USBD_MSC_IN_OUT_MAX_SIZE)
        };
        memcpy(desc, msc_desc, sizeof(msc_desc));
        desc += sizeof(msc_desc);
    }
    #endif

    #if CFG_TUD_NCM
    if (flags & 0x04) {  // USB_BUILTIN_FLAG_NCM
        const uint8_t ncm_desc[] = {
            TUD_CDC_NCM_DESCRIPTOR(USBD_ITF_NET, USBD_STR_NET, USBD_STR_NET_MAC, USBD_NET_EP_CMD, 64, USBD_NET_EP_OUT, USBD_NET_EP_IN, USBD_NET_IN_OUT_MAX_SIZE, CFG_TUD_NET_MTU)
        };
        memcpy(desc, ncm_desc, sizeof(ncm_desc));
        desc += sizeof(ncm_desc);
    }
    #endif

    return desc_cfg_buffer;
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
    if (mp_usbd_class_state.ncm_enabled && CFG_TUD_NCM) {
        len += TUD_CDC_NCM_DESC_LEN;
    }

    return len;
}

// Get number of enabled interfaces
static uint8_t mp_usbd_get_interface_count(void) {
    uint8_t count = 0;

    if (mp_usbd_class_state.cdc_enabled && CFG_TUD_CDC) {
        count += 2;  // CDC uses 2 interfaces
    }
    if (mp_usbd_class_state.msc_enabled && CFG_TUD_MSC) {
        count += 1;
    }
    if (mp_usbd_class_state.ncm_enabled && CFG_TUD_NCM) {
        count += 1;
    }

    return count;
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
    #if CFG_TUD_NCM
    // Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
    TUD_CDC_NCM_DESCRIPTOR(USBD_ITF_NET, USBD_STR_NET, USBD_STR_NET_MAC, USBD_NET_EP_CMD, 64, USBD_NET_EP_OUT, USBD_NET_EP_IN, USBD_NET_IN_OUT_MAX_SIZE, CFG_TUD_NET_MTU),
    #endif
};
#endif


// Dynamic descriptor buffer for runtime configuration
static uint8_t mp_usbd_dynamic_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN];

// Generate dynamic configuration descriptor based on enabled classes
static const uint8_t *mp_usbd_generate_desc_cfg(void) {
    uint8_t *desc = mp_usbd_dynamic_desc_cfg;
    uint8_t interface_count = mp_usbd_get_interface_count();
    size_t total_len = mp_usbd_get_descriptor_cfg_len();

    // Configuration descriptor header
    *desc++ = 9;                           // bLength
    *desc++ = TUSB_DESC_CONFIGURATION;     // bDescriptorType
    *desc++ = (total_len) & 0xFF;          // wTotalLength low
    *desc++ = (total_len >> 8) & 0xFF;     // wTotalLength high
    *desc++ = interface_count;             // bNumInterfaces
    *desc++ = 1;                           // bConfigurationValue
    *desc++ = USBD_STR_0;                  // iConfiguration
    *desc++ = 0;                           // bmAttributes
    *desc++ = USBD_MAX_POWER_MA / 2;       // bMaxPower (in 2mA units)

    // Add enabled class descriptors
    #if CFG_TUD_CDC
    if (mp_usbd_class_state.cdc_enabled) {
        const uint8_t cdc_desc[] = {
            TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
                USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE)
        };
        memcpy(desc, cdc_desc, sizeof(cdc_desc));
        desc += sizeof(cdc_desc);
    }
    #endif

    #if CFG_TUD_MSC
    if (mp_usbd_class_state.msc_enabled) {
        const uint8_t msc_desc[] = {
            TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_EP_OUT, USBD_MSC_EP_IN, USBD_MSC_IN_OUT_MAX_SIZE)
        };
        memcpy(desc, msc_desc, sizeof(msc_desc));
        desc += sizeof(msc_desc);
    }
    #endif

    #if CFG_TUD_NCM
    if (mp_usbd_class_state.ncm_enabled) {
        const uint8_t ncm_desc[] = {
            TUD_CDC_NCM_DESCRIPTOR(USBD_ITF_NET, USBD_STR_NET, USBD_STR_NET_MAC, USBD_NET_EP_CMD, 64, USBD_NET_EP_OUT, USBD_NET_EP_IN, USBD_NET_IN_OUT_MAX_SIZE, CFG_TUD_NET_MTU)
        };
        memcpy(desc, ncm_desc, sizeof(ncm_desc));
        desc += sizeof(ncm_desc);
    }
    #endif

    return mp_usbd_dynamic_desc_cfg;
}
#endif

// For backward compatibility - return the maximum static descriptor
const uint8_t mp_usbd_builtin_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, USBD_ITF_BUILTIN_MAX, USBD_STR_0, MP_USBD_BUILTIN_DESC_CFG_LEN,
        0, USBD_MAX_POWER_MA),

    #if CFG_TUD_CDC
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_EP_OUT, USBD_MSC_EP_IN, USBD_MSC_IN_OUT_MAX_SIZE),
    #endif
    #if CFG_TUD_NCM
    // Interface number, description string index, MAC address string index, EP notification address and size, EP data address (out, in), and size, max segment size.
    TUD_CDC_NCM_DESCRIPTOR(USBD_ITF_NET, USBD_STR_NET, USBD_STR_NET_MAC, USBD_NET_EP_CMD, 64, USBD_NET_EP_OUT, USBD_NET_EP_IN, USBD_NET_IN_OUT_MAX_SIZE, CFG_TUD_NET_MTU),
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
            #if CFG_TUD_NCM
            case USBD_STR_NET:
                desc_str = MICROPY_HW_NETWORK_USBNET_INTERFACE_STRING;
                break;
            case USBD_STR_NET_MAC:
                mp_usbd_hex_str(serial_buf, (uint8_t *)tud_network_mac_address, sizeof(tud_network_mac_address));
                desc_str = serial_buf;
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
