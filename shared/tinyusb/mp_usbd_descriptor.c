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
#define USBD_NCM_IN_OUT_MAX_SIZE ((CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED) ? 512 : 64)

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

#if (CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED)
// Device qualifier descriptor for high-speed devices.
const tusb_desc_device_qualifier_t mp_usbd_builtin_desc_qual = {
    .bLength = sizeof(tusb_desc_device_qualifier_t),
    .bDescriptorType = TUSB_DESC_DEVICE_QUALIFIER,
    .bcdUSB = 0x0200,
    .bDeviceClass = TUSB_CLASS_MISC,
    .bDeviceSubClass = MISC_SUBCLASS_COMMON,
    .bDeviceProtocol = MISC_PROTOCOL_IAD,
    .bMaxPacketSize0 = CFG_TUD_ENDPOINT0_SIZE,
    .bNumConfigurations = 0x01,
    .bReserved = 0x00,
};
#endif

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
    TUD_CDC_NCM_DESCRIPTOR(USBD_ITF_NCM, USBD_STR_NCM, USBD_STR_NCM_MAC, USBD_NCM_EP_CMD, 64, USBD_NCM_EP_OUT, USBD_NCM_EP_IN, USBD_NCM_IN_OUT_MAX_SIZE, CFG_TUD_NET_MTU),
    #endif
};

#if CFG_TUD_CDC
// Default descriptor: CDC + MSC when compiled; NCM requires opt-in via boot.py.
const uint8_t mp_usbd_default_desc_cfg[MP_USBD_DEFAULT_DESC_CFG_LEN] = {
    TUD_CONFIG_DESCRIPTOR(1, 2 + (CFG_TUD_MSC ? 1 : 0), USBD_STR_0, MP_USBD_DEFAULT_DESC_CFG_LEN,
        0, USBD_MAX_POWER_MA),
    TUD_CDC_DESCRIPTOR(USBD_ITF_CDC, USBD_STR_CDC, USBD_CDC_EP_CMD,
        USBD_CDC_CMD_MAX_SIZE, USBD_CDC_EP_OUT, USBD_CDC_EP_IN, USBD_CDC_IN_OUT_MAX_SIZE),
    #if CFG_TUD_MSC
    TUD_MSC_DESCRIPTOR(USBD_ITF_MSC, USBD_STR_MSC, USBD_MSC_EP_OUT, USBD_MSC_EP_IN, USBD_MSC_IN_OUT_MAX_SIZE),
    #endif
};
#endif

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
            case USBD_STR_NCM:
                desc_str = MICROPY_PY_NETWORK_NCM_INTERFACE_STRING;
                break;
            case USBD_STR_NCM_MAC:
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

// Byte offsets within each class descriptor fragment that contain
// interface numbers or endpoint addresses, derived from TUD_*_DESCRIPTOR
// macro layouts in TinyUSB (lib/tinyusb, commit aa0fc2e08).
// The asserts in mp_usbd_build_cfg_desc() verify the first patched byte
// of each class against the expected value; a TinyUSB update that changes
// descriptor layouts will trigger a build failure.

#if CFG_TUD_CDC
static const uint8_t cdc_itf_patch[] = {2, 10, 26, 34, 35, 45};
static const uint8_t cdc_ep_patch[] = {38, 54, 61};
#endif

#if CFG_TUD_MSC
static const uint8_t msc_itf_patch[] = {2};
static const uint8_t msc_ep_patch[] = {11, 18};
#endif

#if CFG_TUD_NCM
static const uint8_t ncm_itf_patch[] = {2, 10, 25, 26, 55, 64};
static const uint8_t ncm_ep_patch[] = {48, 73, 80};
#endif

// Apply interface and endpoint offset deltas to a copied descriptor fragment.
static void patch_desc_frag(uint8_t *frag,
    const uint8_t *itf_off, uint8_t n_itf,
    const uint8_t *ep_off, uint8_t n_ep,
    int8_t itf_delta, int8_t ep_delta) {
    for (uint8_t i = 0; i < n_itf; i++) {
        frag[itf_off[i]] += itf_delta;
    }
    for (uint8_t i = 0; i < n_ep; i++) {
        frag[ep_off[i]] += ep_delta;
    }
}

uint16_t mp_usbd_build_cfg_desc(uint8_t class_mask, uint8_t *buf,
    uint8_t *out_itf_max, uint8_t *out_ep_max, uint8_t *out_str_max) {
    // Verify every patched byte position in each class descriptor fragment
    // against the expected interface number or endpoint address from the builtin
    // descriptor.  These assertions catch TinyUSB descriptor layout changes
    // (lib/tinyusb commit aa0fc2e08) at compile time.
    #if CFG_TUD_CDC
    #define _CDC (TUD_CONFIG_DESC_LEN)
    // Interface number patches: IAD iFirstInterface, comm interface, call mgmt
    // data interface, union master/slave, data interface.
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[0]] == USBD_ITF_CDC);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[1]] == USBD_ITF_CDC);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[2]] == USBD_ITF_CDC_I2);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[3]] == USBD_ITF_CDC);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[4]] == USBD_ITF_CDC_I2);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_itf_patch[5]] == USBD_ITF_CDC_I2);
    // Endpoint address patches: notification, bulk-out, bulk-in.
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_ep_patch[0]] == USBD_CDC_EP_CMD);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_ep_patch[1]] == USBD_CDC_EP_OUT);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_CDC + cdc_ep_patch[2]] == USBD_CDC_EP_IN);
#undef _CDC
    #endif
    #if CFG_TUD_MSC
    #define _MSC (TUD_CONFIG_DESC_LEN + (CFG_TUD_CDC ? TUD_CDC_DESC_LEN : 0))
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_MSC + msc_itf_patch[0]] == USBD_ITF_MSC);
    // Endpoint address patches: bulk-out, bulk-in.
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_MSC + msc_ep_patch[0]] == USBD_MSC_EP_OUT);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_MSC + msc_ep_patch[1]] == USBD_MSC_EP_IN);
#undef _MSC
    #endif
    #if CFG_TUD_NCM
    #define _NCM (TUD_CONFIG_DESC_LEN + (CFG_TUD_CDC ? TUD_CDC_DESC_LEN : 0) + (CFG_TUD_MSC ? TUD_MSC_DESC_LEN : 0))
    // Interface patches: IAD iFirstInterface, comm interface, union master,
    // union slave, data alt=0 interface, data alt=1 interface.
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[0]] == USBD_ITF_NCM);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[1]] == USBD_ITF_NCM);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[2]] == USBD_ITF_NCM);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[3]] == USBD_ITF_NCM_DATA);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[4]] == USBD_ITF_NCM_DATA);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_itf_patch[5]] == USBD_ITF_NCM_DATA);
    // Endpoint address patches: notification, bulk-in, bulk-out.
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_ep_patch[0]] == USBD_NCM_EP_CMD);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_ep_patch[1]] == USBD_NCM_EP_IN);
    MP_STATIC_ASSERT_NONCONSTEXPR(mp_usbd_builtin_desc_cfg[_NCM + ncm_ep_patch[2]] == USBD_NCM_EP_OUT);
#undef _NCM
    #endif
    // Verify config descriptor header length matches USB 2.0 §9.6.3.
    MP_STATIC_ASSERT(TUD_CONFIG_DESC_LEN == 9);

    // Copy selected class fragments from mp_usbd_builtin_desc_cfg and
    // patch interface/endpoint numbers for compact numbering (no gaps).
    uint8_t *p = buf + TUD_CONFIG_DESC_LEN;
    const uint8_t *src = mp_usbd_builtin_desc_cfg + TUD_CONFIG_DESC_LEN;
    uint8_t num_itf = 0;
    uint8_t itf = 0;
    uint8_t ep = 1;
    uint8_t str_max = USBD_STR_SERIAL + 1;

    #if CFG_TUD_CDC
    if (class_mask & MP_USBD_FLAG_CDC) {
        memcpy(p, src, TUD_CDC_DESC_LEN);
        patch_desc_frag(p,
            cdc_itf_patch, sizeof(cdc_itf_patch),
            cdc_ep_patch, sizeof(cdc_ep_patch),
            itf - USBD_ITF_CDC, ep - (USBD_CDC_EP_CMD & 0x7F));
        p += TUD_CDC_DESC_LEN;
        itf += 2;
        ep += 2;
        num_itf += 2;
        str_max = USBD_STR_CDC + 1;
    }
    src += TUD_CDC_DESC_LEN;
    #endif

    #if CFG_TUD_MSC
    if (class_mask & MP_USBD_FLAG_MSC) {
        memcpy(p, src, TUD_MSC_DESC_LEN);
        patch_desc_frag(p,
            msc_itf_patch, sizeof(msc_itf_patch),
            msc_ep_patch, sizeof(msc_ep_patch),
            itf - USBD_ITF_MSC, ep - (USBD_MSC_EP_IN & 0x7F));
        p += TUD_MSC_DESC_LEN;
        itf += 1;
        ep += 1;
        num_itf += 1;
        str_max = USBD_STR_MSC + 1;
    }
    src += TUD_MSC_DESC_LEN;
    #endif

    #if CFG_TUD_NCM
    if (class_mask & MP_USBD_FLAG_NCM) {
        memcpy(p, src, TUD_CDC_NCM_DESC_LEN);
        patch_desc_frag(p,
            ncm_itf_patch, sizeof(ncm_itf_patch),
            ncm_ep_patch, sizeof(ncm_ep_patch),
            itf - USBD_ITF_NCM, ep - (USBD_NCM_EP_CMD & 0x7F));
        p += TUD_CDC_NCM_DESC_LEN;
        itf += 2;
        ep += 2;
        num_itf += 2;
        str_max = USBD_STR_NCM_MAC + 1;
    }
    src += TUD_CDC_NCM_DESC_LEN;
    #endif

    (void)src;

    // Caller is expected to validate class_mask before calling, but guard
    // here too: returning a header-only descriptor confuses the USB host.
    assert(num_itf > 0);

    // Check endpoint count fits TinyUSB's compiled limit.
    assert(ep <= CFG_TUD_ENDPPOINT_MAX);

    // Write config descriptor header now that total length is known.
    // TUD_CONFIG_DESCRIPTOR requires compile-time constant args and cannot be
    // used here; fields are written individually using the USB 2.0 §9.6.3 layout.
    uint16_t total_len = p - buf;
    buf[0] = TUD_CONFIG_DESC_LEN;
    buf[1] = TUSB_DESC_CONFIGURATION;
    buf[2] = total_len & 0xFF;
    buf[3] = (total_len >> 8) & 0xFF;
    buf[4] = num_itf;
    buf[5] = 1;
    buf[6] = USBD_STR_0;
    buf[7] = 0x80;
    buf[8] = USBD_MAX_POWER_MA / 2;

    if (out_itf_max) {
        *out_itf_max = itf;
    }
    if (out_ep_max) {
        *out_ep_max = ep;
    }
    if (out_str_max) {
        *out_str_max = str_max;
    }

    return total_len;
}

#if !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

const uint8_t *tud_descriptor_device_cb(void) {
    return (const void *)&mp_usbd_builtin_desc_dev;
}

const uint8_t *tud_descriptor_configuration_cb(uint8_t index) {
    (void)index;
    // Without RUNTIME_DEVICE, there is no machine.USBDevice to select
    // classes from boot.py, so expose all compiled-in classes.
    return mp_usbd_builtin_desc_cfg;
}

#if (CFG_TUD_MAX_SPEED == OPT_MODE_HIGH_SPEED)
uint8_t const *tud_descriptor_device_qualifier_cb(void) {
    return (uint8_t const *)&mp_usbd_builtin_desc_qual;
}
#endif

#else

// If runtime device support is enabled, descriptor callbacks are implemented in usbd.c

#endif // !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

#endif // MICROPY_HW_ENABLE_USBDEV
