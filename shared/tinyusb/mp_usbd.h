/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
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

#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_H

#include "py/mpconfig.h"

#if MICROPY_HW_ENABLE_USBDEV

#include "py/obj.h"
#include "py/objarray.h"
#include "py/runtime.h"

#ifndef NO_QSTR
#include "tusb.h"
#include "device/dcd.h"
#include "class/cdc/cdc_device.h"
#endif

// Run the TinyUSB device task
void mp_usbd_task(void);

// Schedule a call to mp_usbd_task(), even if no USB interrupt has occurred
void mp_usbd_schedule_task(void);

// Function to be implemented in port code.
// Can write a string up to MICROPY_HW_USB_DESC_STR_MAX characters long, plus terminating byte.
extern void mp_usbd_port_get_serial_number(char *buf);

// Most ports need to write a hexadecimal serial number from a byte array. This
// is a helper function for this. out_str must be long enough to hold a string of total
// length (2 * bytes_len + 1) (including NUL terminator).
void mp_usbd_hex_str(char *out_str, const uint8_t *bytes, size_t bytes_len);

// Per-class runtime enable/disable state
typedef struct {
    bool cdc_enabled;
    bool msc_enabled;
} mp_usbd_class_state_t;

// Global class enable state
extern mp_usbd_class_state_t mp_usbd_class_state;

// Descriptor template entry for lookup table
typedef struct {
    uint8_t flag_mask;     // USB_BUILTIN_FLAG_* to check
    uint8_t desc_len;      // Length of descriptor in bytes
    uint8_t itf_count;     // Number of interfaces used
    const uint8_t *template; // Descriptor bytes (0xFF = interface number placeholder)
} usb_desc_entry_t;

// External reference to descriptor lookup table for inline functions
extern const usb_desc_entry_t usb_desc_table[];

// Number of entries in usb_desc_table
#define USB_DESC_TABLE_SIZE ((CFG_TUD_CDC ? 1 : 0) + (CFG_TUD_MSC ? 1 : 0))

// Descriptor info structure for combined length/count calculation
typedef struct {
    size_t length;
    uint8_t interface_count;
} usb_desc_info_t;

// Allow runtime override of VID/PID defaults
#ifndef MICROPY_HW_USB_RUNTIME_VID
#define MICROPY_HW_USB_RUNTIME_VID MICROPY_HW_USB_VID
#endif
#ifndef MICROPY_HW_USB_RUNTIME_PID
#define MICROPY_HW_USB_RUNTIME_PID MICROPY_HW_USB_PID
#endif

// Forward declarations for USBBuiltin type
typedef struct _mp_obj_usb_builtin_t mp_obj_usb_builtin_t;
extern const mp_obj_type_t mp_type_usb_builtin;

// Individual USB class flags for bitfield operations
#define USB_BUILTIN_FLAG_NONE  0x00
#define USB_BUILTIN_FLAG_CDC   0x01
#define USB_BUILTIN_FLAG_MSC   0x02
#define USB_BUILTIN_FLAG_NCM   0x04

// Initialize class state based on compile-time configuration and runtime mode
static inline void mp_usbd_init_class_state(void) {
    #if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
    // In runtime mode, only CDC enabled by default
    mp_usbd_class_state.cdc_enabled = (CFG_TUD_CDC == 1);
    mp_usbd_class_state.msc_enabled = false;
    #else
    // In static mode, enable all compiled classes
    mp_usbd_class_state.cdc_enabled = (CFG_TUD_CDC == 1);
    mp_usbd_class_state.msc_enabled = (CFG_TUD_MSC == 1);
    #endif
}

// Update class state based on bitfield flags
static inline void mp_usbd_update_class_state(uint8_t flags) {
    mp_usbd_class_state.cdc_enabled = (flags & USB_BUILTIN_FLAG_CDC) && (CFG_TUD_CDC == 1);
    mp_usbd_class_state.msc_enabled = (flags & USB_BUILTIN_FLAG_MSC) && (CFG_TUD_MSC == 1);
}

// Calculate descriptor length from flags using compile-time conditionals
static inline size_t mp_usbd_get_descriptor_cfg_len_from_flags(uint8_t flags) {
    size_t len = TUD_CONFIG_DESC_LEN;
    #if CFG_TUD_CDC
    if (flags & USB_BUILTIN_FLAG_CDC) {
        len += TUD_CDC_DESC_LEN;
    }
    #endif
    #if CFG_TUD_MSC
    if (flags & USB_BUILTIN_FLAG_MSC) {
        len += TUD_MSC_DESC_LEN;
    }
    #endif
    return len;
}

// Calculate interface count from flags using compile-time conditionals
static inline uint8_t mp_usbd_get_interface_count_from_flags(uint8_t flags) {
    uint8_t count = 0;
    #if CFG_TUD_CDC
    if (flags & USB_BUILTIN_FLAG_CDC) {
        count += 2;  // CDC uses 2 interfaces
    }
    #endif
    #if CFG_TUD_MSC
    if (flags & USB_BUILTIN_FLAG_MSC) {
        count += 1;  // MSC uses 1 interface
    }
    #endif
    return count;
}

// Combined descriptor info calculation using compile-time conditionals
static inline usb_desc_info_t mp_usbd_get_desc_info_from_flags(uint8_t flags) {
    usb_desc_info_t info = { .length = TUD_CONFIG_DESC_LEN, .interface_count = 0 };
    #if CFG_TUD_CDC
    if (flags & USB_BUILTIN_FLAG_CDC) {
        info.length += TUD_CDC_DESC_LEN;
        info.interface_count += 2;
    }
    #endif
    #if CFG_TUD_MSC
    if (flags & USB_BUILTIN_FLAG_MSC) {
        info.length += TUD_MSC_DESC_LEN;
        info.interface_count += 1;
    }
    #endif
    return info;
}

// Initialise TinyUSB device
static inline void mp_usbd_init_tud(void) {
    // Initialize class state before TinyUSB init
    mp_usbd_init_class_state();

    tusb_init();
    #if MICROPY_HW_USB_CDC
    tud_cdc_configure_fifo_t cfg = { .rx_persistent = 0,
                                     .tx_persistent = 1,

                                     // This config flag is unreleased in TinyUSB >v0.18.0
                                     // but included in Espressif's TinyUSB component since v0.18.0~3
                                     //
                                     // Versioning issue reported as
                                     // https://github.com/espressif/esp-usb/issues/236
                                     #if TUSB_VERSION_NUMBER > 1800 || defined(ESP_PLATFORM)
                                     .tx_overwritabe_if_not_connected = 1,
                                     #endif
    };
    tud_cdc_configure_fifo(&cfg);
    #endif
}

// Get dynamic descriptor length based on enabled classes
size_t mp_usbd_get_descriptor_cfg_len(void);

// Length of built-in configuration descriptor
#define MP_USBD_BUILTIN_DESC_CFG_LEN (TUD_CONFIG_DESC_LEN +                     \
    (CFG_TUD_CDC ? (TUD_CDC_DESC_LEN) : 0) +  \
    (CFG_TUD_MSC ? (TUD_MSC_DESC_LEN) : 0)    \
    )

// Built-in USB device and configuration descriptor values
extern const tusb_desc_device_t mp_usbd_builtin_desc_dev;
extern const uint8_t mp_usbd_builtin_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN];

void mp_usbd_task_callback(mp_sched_node_t *node);

#if !MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE

static inline void mp_usbd_init(void) {
    // Without runtime USB support, this can be a thin wrapper wrapper around tusb_init()
    // which is called in the below helper function.
    mp_usbd_init_tud();
}

static inline void mp_usbd_deinit(void) {
    // Called in soft reset path. No-op if no runtime USB devices require cleanup.
}

#else
// Runtime USB Device support requires more complex init/deinit
void mp_usbd_init(void);
void mp_usbd_deinit(void);

const char *mp_usbd_runtime_string_cb(uint8_t index);

// Maximum number of pending exceptions per single TinyUSB task execution
#define MP_USBD_MAX_PEND_EXCS 2

// Full runtime USB device structure
typedef struct {
    mp_obj_base_t base;

    mp_obj_t desc_dev; // Device descriptor bytes
    mp_obj_t desc_cfg; // Configuration descriptor bytes
    mp_obj_t desc_strs; // List/dict/similar to look up string descriptors by index

    // Runtime device driver callback functions
    mp_obj_t open_itf_cb;
    mp_obj_t reset_cb;
    mp_obj_t control_xfer_cb;
    mp_obj_t xfer_cb;

    uint8_t builtin_driver; // Bitfield of USB_BUILTIN_FLAG_* values

    bool active; // Has the user set the USB device active?
    bool trigger; // Has the user requested the active state change (or re-activate)?

    // Temporary pointers for xfer data in progress on each endpoint
    // Ensuring they aren't garbage collected until the xfer completes
    mp_obj_t xfer_data[CFG_TUD_ENDPPOINT_MAX][2];

    // Pointer to a memoryview that is reused to refer to various pieces of
    // control transfer data that are pushed to USB control transfer
    // callbacks. Python code can't rely on the memoryview contents
    // to remain valid after the callback returns!
    mp_obj_array_t *control_data;

    // Pointers to exceptions thrown inside Python callbacks. See
    // usbd_callback_function_n().
    mp_uint_t num_pend_excs;
    mp_obj_t pend_excs[MP_USBD_MAX_PEND_EXCS];
} mp_obj_usb_device_t;

#else // Static USBD drivers only

// Minimal USB device structure for static mode (builtin_driver control only)
typedef struct {
    mp_obj_base_t base;
    mp_obj_t builtin_driver; // Points to a USBBuiltin constant object
    bool active; // Has the user set the USB device active?
} mp_obj_usb_device_t;

#endif

// Return true if any built-in driver is enabled
bool mp_usb_device_builtin_enabled(const mp_obj_usb_device_t *usbd);

#endif // MICROPY_HW_ENABLE_USBDEV

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_USBD_H
