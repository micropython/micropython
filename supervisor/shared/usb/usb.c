// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/objstr.h"
#include "supervisor/background_callback.h"
#include "supervisor/linker.h"
#include "supervisor/usb.h"
#include "shared/readline/readline.h"

#if CIRCUITPY_STATUS_BAR
#include "supervisor/shared/status_bar.h"
#endif

#if CIRCUITPY_STORAGE
#include "shared-module/storage/__init__.h"
#endif

#if CIRCUITPY_USB_DEVICE
#include "shared-bindings/supervisor/__init__.h"

#if CIRCUITPY_USB_CDC
#include "shared-module/usb_cdc/__init__.h"
#endif

#if CIRCUITPY_USB_HID
#include "shared-module/usb_hid/__init__.h"
#endif

#if CIRCUITPY_USB_MIDI
#include "shared-module/usb_midi/__init__.h"
#endif

#if CIRCUITPY_USB_VIDEO
#include "shared-module/usb_video/__init__.h"
#endif

#endif

#include "tusb.h"

bool usb_enabled(void) {
    return tusb_inited();
}

MP_WEAK void post_usb_init(void) {
}

void usb_init(void) {
    #if CIRCUITPY_USB_DEVICE
    usb_identification_t defaults;
    usb_identification_t *identification;
    #if CIRCUITPY_USB_IDENTIFICATION
    if (custom_usb_identification != NULL) {
        identification = custom_usb_identification;
    } else {
    #endif
    // This compiles to less code than using a struct initializer.
    defaults.vid = USB_VID;
    defaults.pid = USB_PID;
    strcpy(defaults.manufacturer_name, USB_MANUFACTURER);
    strcpy(defaults.product_name, USB_PRODUCT);
    identification = &defaults;
    // This memory only needs to be live through the end of usb_build_descriptors.
    #if CIRCUITPY_USB_IDENTIFICATION
}
    #endif
    if (!usb_build_descriptors(identification)) {
        return;
    }
    init_usb_hardware();
    #if CIRCUITPY_USB_HID
    usb_hid_build_report_descriptor();
    #endif

    // Only init device. Host gets inited by the `usb_host` module common-hal.
    tud_init(TUD_OPT_RHPORT);
    #endif

    post_usb_init();

    #if MICROPY_KBD_EXCEPTION && CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
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
    #if CIRCUITPY_USB_DEVICE
    #if CIRCUITPY_STORAGE && CIRCUITPY_USB_MSC
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
    #endif
};

// Call this when ready to run code.py or a REPL, and a VM has been started.
void usb_setup_with_vm(void) {
    #if CIRCUITPY_USB_DEVICE
    #if CIRCUITPY_USB_HID
    usb_hid_setup_devices();
    #endif

    #if CIRCUITPY_USB_MIDI
    usb_midi_setup_ports();
    #endif
    #endif
}

void usb_background(void) {
    if (usb_enabled()) {
        #if CFG_TUSB_OS == OPT_OS_NONE || CFG_TUSB_OS == OPT_OS_PICO
        tud_task();
        #if CIRCUITPY_USB_HOST || CIRCUITPY_MAX3421E
        tuh_task();
        #endif
        #elif CFG_TUSB_OS == OPT_OS_FREERTOS
        // Yield to FreeRTOS in case TinyUSB runs in a separate task. Don't use
        // port_yield() because it has a longer delay.
        vTaskDelay(0);
        #endif
        // No need to flush if there's no REPL.
        #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_CDC
        if (usb_cdc_console_enabled()) {
            // Console will always be itf 0.
            tud_cdc_write_flush();
        }
        #endif
        #if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_VIDEO
        usb_video_task();
        #endif
    }
}

static background_callback_t usb_callback;
static void usb_background_do(void *unused) {
    usb_background();
}

void PLACE_IN_ITCM(usb_background_schedule)(void) {
    background_callback_add(&usb_callback, usb_background_do, NULL);
}

void PLACE_IN_ITCM(usb_irq_handler)(int instance) {
    #if CFG_TUSB_MCU != OPT_MCU_RP2040
    #if CIRCUITPY_USB_DEVICE
    // For rp2040, IRQ handler is already installed and invoked automatically
    if (instance == CIRCUITPY_USB_DEVICE_INSTANCE) {
        tud_int_handler(instance);
    }
    #endif
    #if CIRCUITPY_USB_HOST
    if (instance == CIRCUITPY_USB_HOST_INSTANCE) {
        tuh_int_handler(instance);
    }
    #endif
    #endif

    usb_background_schedule();
}
