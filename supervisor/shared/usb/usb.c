/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 hathach for Adafruit Industries
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

#include "py/objstr.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Processor.h"
#include "supervisor/background_callback.h"
#include "supervisor/port.h"
#include "supervisor/serial.h"
#include "supervisor/usb.h"
#include "supervisor/shared/workflow.h"
#include "lib/utils/interrupt_char.h"
#include "lib/mp-readline/readline.h"

#if CIRCUITPY_STORAGE
#include "shared-module/storage/__init__.h"
#endif

#if CIRCUITPY_USB_CDC
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_USB_HID
#include "shared-module/usb_hid/__init__.h"
#endif

#if CIRCUITPY_USB_MIDI
#include "shared-module/usb_midi/__init__.h"
#endif

#include "tusb.h"

#if CIRCUITPY_USB_VENDOR

// The WebUSB support being conditionally added to this file is based on the
// tinyusb demo examples/device/webusb_serial.

extern const tusb_desc_webusb_url_t desc_webusb_url;

static bool web_serial_connected = false;
#endif

bool usb_enabled(void) {
    return tusb_inited();
}

MP_WEAK void post_usb_init(void) {
}

void usb_init(void) {
    init_usb_hardware();

    tusb_init();

    post_usb_init();

    #if MICROPY_KBD_EXCEPTION && CIRCUITPY_USB_CDC
    // Set Ctrl+C as wanted char, tud_cdc_rx_wanted_cb() usb_callback will be invoked when Ctrl+C is received
    // This usb_callback always got invoked regardless of mp_interrupt_char value since we only set it once here

    // Don't watch for ctrl-C if there is no REPL.
    if (usb_cdc_console_enabled()) {
        // Console will always be itf 0.
        tud_cdc_set_wanted_char(CHAR_CTRL_C);
    }
    #endif
}

// Set up USB defaults before any USB changes are made in boot.py
void usb_set_defaults(void) {
    #if CIRCUITPY_STORAGE
    storage_usb_set_defaults();
    #endif

    #if CIRCUITPY_USB_CDC
    usb_cdc_set_defaults();
    #endif

    #if CIRCUITPY_USB_HID
    usb_hid_set_defaults();
    #endif

    #if CIRCUITPY_USB_MIDI
    usb_midi_set_defaults();
    #endif
};

// Some dynamic USB data must be saved after boot.py. How much is needed?
size_t usb_boot_py_data_size(void) {
    size_t size = 0;

    #if CIRCUITPY_USB_HID
    size += usb_hid_report_descriptor_length();
    #endif

    return size;
}

// Fill in the data to save.
void usb_get_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size) {
    #if CIRCUITPY_USB_HID
    usb_hid_build_report_descriptor(temp_storage, temp_storage_size);
    #endif
}

// After VM is gone, save data into non-heap storage (storage_allocations).
void usb_return_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size) {
    #if CIRCUITPY_USB_HID
    usb_hid_save_report_descriptor(temp_storage, temp_storage_size);
    #endif

    // Now we can also build the rest of the descriptors and place them in storage_allocations.
    usb_build_descriptors();
}

// Call this when ready to run code.py or a REPL, and a VM has been started.
void usb_setup_with_vm(void) {
    #if CIRCUITPY_USB_HID
    usb_hid_setup_devices();
    #endif

    #if CIRCUITPY_USB_MIDI
    usb_midi_setup_ports();
    #endif
}

void usb_disconnect(void) {
    tud_disconnect();
}

void usb_background(void) {
    if (usb_enabled()) {
        #if CFG_TUSB_OS == OPT_OS_NONE
        tud_task();
        #endif
        // No need to flush if there's no REPL.
        #if CIRCUITPY_USB_CDC
        if (usb_cdc_console_enabled()) {
            // Console will always be itf 0.
            tud_cdc_write_flush();
        }
        #endif
    }
}

static background_callback_t usb_callback;
static void usb_background_do(void *unused) {
    usb_background();
}

void usb_background_schedule(void) {
    background_callback_add(&usb_callback, usb_background_do, NULL);
}

void usb_irq_handler(void) {
    tud_int_handler(0);
    usb_background_schedule();
}

// --------------------------------------------------------------------+
// tinyusb callbacks
// --------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
    #if CIRCUITPY_USB_MSC
    usb_msc_mount();
    #endif
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
    #if CIRCUITPY_USB_MSC
    usb_msc_umount();
    #endif
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allows us to perform remote wakeup
// USB Specs: Within 7ms, device must draw an average current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}

// Invoked when cdc when line state changed e.g connected/disconnected
// Use to reset to DFU when disconnect with 1200 bps
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void)itf;  // interface ID, not used

    // DTR = false is counted as disconnected
    if (!dtr) {
        cdc_line_coding_t coding;
        // Use whichever CDC is itf 0.
        tud_cdc_get_line_coding(&coding);

        if (coding.bit_rate == 1200) {
            reset_to_bootloader();
        }
    }
}

#if CIRCUITPY_USB_VENDOR
// --------------------------------------------------------------------+
// WebUSB use vendor class
// --------------------------------------------------------------------+

bool tud_vendor_connected(void) {
    return web_serial_connected;
}

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const *request) {
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP) {
        return true;
    }

    switch (request->bRequest)
    {
        case VENDOR_REQUEST_WEBUSB:
            // match vendor request in BOS descriptor
            // Get landing page url
            return tud_control_xfer(rhport, request, (void *)&desc_webusb_url, desc_webusb_url.bLength);

        case VENDOR_REQUEST_MICROSOFT:
            if (request->wIndex == 7) {
                // Get Microsoft OS 2.0 compatible descriptor
                uint16_t total_len;
                memcpy(&total_len, desc_ms_os_20 + 8, 2);

                return tud_control_xfer(rhport, request, (void *)desc_ms_os_20, total_len);
            } else {
                return false;
            }

        case 0x22:
            // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
            // connect and disconnect.
            web_serial_connected = (request->wValue != 0);

            // response with status OK
            return tud_control_status(rhport, request);

        default:
            // stall unknown request
            return false;
    }

    return true;
}
#endif // CIRCUITPY_USB_VENDOR


#if MICROPY_KBD_EXCEPTION

/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */

// Only called when console is enabled.
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    // Workaround for using lib/utils/interrupt_char.c
    // Compare mp_interrupt_char with wanted_char and ignore if not matched
    if (mp_interrupt_char == wanted_char) {
        tud_cdc_n_read_flush(itf);    // flush read fifo
        mp_sched_keyboard_interrupt();
    }
}

#endif
