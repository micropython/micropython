// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/runtime.h"
#include "supervisor/port.h"
#include "supervisor/usb.h"
#include "shared/runtime/interrupt_char.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#if CIRCUITPY_USB_CDC
#include "shared-module/usb_cdc/__init__.h"
#include "lib/tinyusb/src/class/cdc/cdc_device.h"
#endif

#if CIRCUITPY_USB_VIDEO
#include "shared-module/usb_video/__init__.h"
#endif

#include "tusb.h"

#if CIRCUITPY_USB_VENDOR
#include "usb_vendor_descriptors.h"

// The WebUSB support being conditionally added to this file is based on the
// tinyusb demo examples/device/webusb_serial.

static bool web_serial_connected = false;

#define URL  "www.tinyusb.org/examples/webusb-serial"

const tusb_desc_webusb_url_t desc_webusb_url =
{
    .bLength = 3 + sizeof(URL) - 1,
    .bDescriptorType = 3, // WEBUSB URL type
    .bScheme = 1,       // 0: http, 1: https
    .url = URL
};

#endif

void usb_disconnect(void) {
    tud_disconnect();
}

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
    } else {
        #if CIRCUITPY_STATUS_BAR
        // We are connected, let's request a title bar update.
        supervisor_status_bar_request_update(true);
        #endif
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
                // let's just hope the target architecture always has the same endianness
                uint16_t total_len;
                memcpy(&total_len, vendor_ms_os_20_descriptor() + 8, 2);

                return tud_control_xfer(rhport, request, (void *)vendor_ms_os_20_descriptor(), total_len);
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


#if MICROPY_KBD_EXCEPTION && CIRCUITPY_USB_CDC

// The CDC RX buffer impacts monitoring for ctrl-c. TinyUSB will only ask for
// more from CDC if the free space in the buffer is greater than the endpoint
// size. Setting CFG_TUD_CDC_RX_BUFSIZE to the endpoint size and then sending
// any character will prevent ctrl-c from working. Require at least a 64
// character buffer.
#if CFG_TUD_CDC_RX_BUFSIZE < CFG_TUD_CDC_EP_BUFSIZE + 64
#error "CFG_TUD_CDC_RX_BUFSIZE must be 64 bytes bigger than endpoint size."
#endif

/**
 * Callback invoked when received an "wanted" char.
 * @param itf           Interface index (for multiple cdc interfaces)
 * @param wanted_char   The wanted char (set previously)
 */
void tud_cdc_rx_wanted_cb(uint8_t itf, char wanted_char) {
    // Workaround for using shared/runtime/interrupt_char.c
    // Compare mp_interrupt_char with wanted_char and ignore if not matched
    if (mp_interrupt_char == wanted_char) {
        tud_cdc_n_read_flush(itf);    // flush read fifo
        mp_sched_keyboard_interrupt();
    }
}

void tud_cdc_send_break_cb(uint8_t itf, uint16_t duration_ms) {
    if (usb_cdc_console_enabled() && mp_interrupt_char != -1 && itf == 0 && duration_ms > 0) {
        mp_sched_keyboard_interrupt();
    }
}

#endif
