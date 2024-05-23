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

#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_CDC_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_CDC_H

#ifndef MICROPY_HW_USB_CDC_TX_TIMEOUT
#define MICROPY_HW_USB_CDC_TX_TIMEOUT (500)
#endif

// This is typically only enabled on esp32
// parts which have an internal usb peripheral.
#ifndef MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER
#define MICROPY_HW_USB_CDC_DTR_RTS_BOOTLOADER (0)
#endif

uintptr_t mp_usbd_cdc_poll_interfaces(uintptr_t poll_flags);
void tud_cdc_rx_cb(uint8_t itf);
mp_uint_t mp_usbd_cdc_tx_strn(const char *str, mp_uint_t len);

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_CDC_H
