// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 hathach for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Ports must call this as frequently as they can in order to keep the USB
// connection alive and responsive.  Normally this is called from background
// tasks after the USB IRQ handler is executed, but in specific circumstances
// it may be necessary to call it directly.
void usb_background(void);

// Schedule usb background
void usb_background_schedule(void);

// Ports must call this from their particular USB IRQ handler
void usb_irq_handler(int instance);

// Only inits the USB peripheral clocks and pins. The peripheral will be initialized by
// TinyUSB.
void init_usb_hardware(void);

// Temporary hook for code after init. Only used for RP2040.
void post_usb_init(void);

// Indexes and counts updated as descriptors are built.
typedef struct {
    size_t current_interface;
    size_t current_endpoint;
    size_t num_in_endpoints;
    size_t num_out_endpoints;
} descriptor_counts_t;

typedef struct {
    uint16_t vid;
    uint16_t pid;
    char manufacturer_name[128];
    char product_name[128];
} usb_identification_t;

// Shared implementation.
bool usb_enabled(void);
void usb_add_interface_string(uint8_t interface_string_index, const char str[]);
bool usb_build_descriptors(const usb_identification_t *identification);
void usb_disconnect(void);
void usb_init(void);
void usb_set_defaults(void);
size_t usb_boot_py_data_size(void);
void usb_get_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size);
void usb_return_boot_py_data(uint8_t *temp_storage, size_t temp_storage_size);

// Further initialization that must be done with a VM present.
void usb_setup_with_vm(void);


// Propagate plug/unplug events to the MSC logic.
#if CIRCUITPY_USB_DEVICE && CIRCUITPY_USB_MSC
void usb_msc_mount(void);
void usb_msc_umount(void);
bool usb_msc_ejected(void);
#endif

#if CIRCUITPY_USB_KEYBOARD_WORKFLOW
void usb_keyboard_init(void);
uint32_t usb_keyboard_chars_available(void);
char usb_keyboard_read_char(void);

bool usb_keyboard_in_use(uint8_t dev_addr, uint8_t interface);
void usb_keyboard_detach(uint8_t dev_addr, uint8_t interface);
void usb_keyboard_attach(uint8_t dev_addr, uint8_t interface);
void usb_keymap_set(const uint8_t *buf, size_t len);
#endif
