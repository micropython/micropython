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

#ifndef MICROPY_INCLUDED_SUPERVISOR_USB_H
#define MICROPY_INCLUDED_SUPERVISOR_USB_H

#include <stdbool.h>

// Ports must call this as frequently as they can in order to keep the USB
// connection alive and responsive.  Normally this is called from background
// tasks after the USB IRQ handler is executed, but in specific circumstances
// it may be necessary to call it directly.
void usb_background(void);

// Schedule usb background
void usb_background_schedule(void);

// Ports must call this from their particular USB IRQ handler
void usb_irq_handler(void);

// Only inits the USB peripheral clocks and pins. The peripheral will be initialized by
// TinyUSB.
void init_usb_hardware(void);

// Temporary hook for code after init. Only used for RP2040.
void post_usb_init(void);

// Shared implementation.
void reset_usb(void);
bool usb_enabled(void);
void usb_init(void);
void usb_disconnect(void);
void usb_gc_collect(void);
void usb_pre_boot_py(void);
void usb_post_boot_py(void);

void usb_add_interface_string(uint8_t interface_string_index, const char str[]);

void reset_usb_desc(void);
void usb_desc_gc_collect(void);
void usb_desc_init(void);
void usb_desc_post_boot_py(void);

// Propagate plug/unplug events to the MSC logic.
#if CIRCUITPY_USB_MSC
void usb_msc_mount(void);
void usb_msc_umount(void);
bool usb_msc_ejected(void);
#endif

#endif // MICROPY_INCLUDED_SUPERVISOR_USB_H
