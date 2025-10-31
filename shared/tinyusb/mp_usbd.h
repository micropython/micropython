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

#if MICROPY_HW_NETWORK_USBNET
#include "extmod/network_usbd_ncm.h"
#endif

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
    bool ncm_enabled;
} mp_usbd_class_state_t;

// Global class enable state
extern mp_usbd_class_state_t mp_usbd_class_state;

// Functions to control USB classes via bitfield flags
void mp_usbd_update_class_state(uint8_t flags);
void mp_usbd_init_class_state(void);

// Initialise TinyUSB device.
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

// Allow runtime override of VID/PID defaults
#ifndef MICROPY_HW_USB_RUNTIME_VID
#define MICROPY_HW_USB_RUNTIME_VID MICROPY_HW_USB_VID
#endif
#ifndef MICROPY_HW_USB_RUNTIME_PID
#define MICROPY_HW_USB_RUNTIME_PID MICROPY_HW_USB_PID
#endif

// Individual USB class flags for bitfield operations
#define USB_BUILTIN_FLAG_NONE  0x00
#define USB_BUILTIN_FLAG_CDC   0x01
#define USB_BUILTIN_FLAG_MSC   0x02
#define USB_BUILTIN_FLAG_NCM   0x04


// Get dynamic descriptor length based on enabled classes
size_t mp_usbd_get_descriptor_cfg_len(void);

// Length of built-in configuration descriptor
#define MP_USBD_BUILTIN_DESC_CFG_LEN ( \
    (CFG_TUD_CDC ? (TUD_CDC_DESC_LEN) : 0) + \
    (CFG_TUD_MSC ? (TUD_MSC_DESC_LEN) : 0) + \
    (CFG_TUD_NCM ? (TUD_CDC_NCM_DESC_LEN) : 0) + \
    TUD_CONFIG_DESC_LEN)

// Built-in USB device and configuration descriptor values
extern const tusb_desc_device_t mp_usbd_builtin_desc_dev;
extern const uint8_t mp_usbd_builtin_desc_cfg[MP_USBD_BUILTIN_DESC_CFG_LEN];

void mp_usbd_task_callback(mp_sched_node_t *node);

#if MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
void mp_usbd_deinit(void);
void mp_usbd_init(void);

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

    mp_obj_t builtin_driver; // Points to one of mp_type_usb_device_builtin_nnn

    bool active; // Has the user set the USB device active?
    bool trigger; // Has the user requested the active state change (or re-activate)?

    uint16_t custom_vid; // Custom VID (0 = use builtin default)
    uint16_t custom_pid; // Custom PID (0 = use builtin default)


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
    mp_obj_t builtin_driver; // Points to one of mp_type_usb_device_builtin_nnn
    bool active; // Has the user set the USB device active?
    uint16_t custom_vid; // Custom VID (0 = use builtin default)
    uint16_t custom_pid; // Custom PID (0 = use builtin default)
} mp_obj_usb_device_t;

static inline void mp_usbd_init(void) {
    // Without runtime USB support, this can be a thin wrapper wrapper around tusb_init()
    // which is called in the below helper function.
    mp_usbd_init_tud();
}

#endif

// Built-in constant objects, possible values of builtin_driver
//
// (Currently not possible to change built-in drivers at runtime, just enable/disable.)
extern const mp_obj_type_t mp_type_usb_device_builtin_default;
extern const mp_obj_type_t mp_type_usb_device_builtin_none;

// Return true if any built-in driver is enabled
static inline bool mp_usb_device_builtin_enabled(const mp_obj_usb_device_t *usbd) {
    return usbd->builtin_driver != MP_OBJ_FROM_PTR(&mp_type_usb_device_builtin_none);
}

#endif // MICROPY_HW_ENABLE_USBDEV

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_USBD_H
