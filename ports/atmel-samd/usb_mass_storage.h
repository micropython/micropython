/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

// This adapts the ASF4 USB mass storage API to MicroPython's VFS API so we can
// expose all VFS block devices as Lun's over USB mass storage control.

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_USB_MASS_STORAGE_H
#define MICROPY_INCLUDED_ATMEL_SAMD_USB_MASS_STORAGE_H

#include <stdint.h>

// "background" task that actually manages loading to and from the file systems.
void usb_msc_background(void);

// Callbacks that hook into ASF4's USB stack.
int32_t usb_msc_disk_eject(uint8_t lun);
int32_t usb_msc_disk_is_writable(uint8_t lun);
int32_t usb_msc_disk_is_ready(uint8_t lun);
int32_t usb_msc_new_read(uint8_t lun, uint32_t addr, uint32_t nblocks);
int32_t usb_msc_new_write(uint8_t lun, uint32_t addr, uint32_t nblocks);
int32_t usb_msc_xfer_done(uint8_t lun);
uint8_t *usb_msc_inquiry_info(uint8_t lun);
uint8_t *usb_msc_get_capacity(uint8_t lun);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_USB_MASS_STORAGE_H
