/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
<<<<<<<< HEAD:shared-bindings/_bleio/ScanResults.h
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
 * Copyright (c) 2018 Artur Pacholec
 * Copyright (c) 2017 Glenn Ruben Bakke
========
 * Copyright (c) 2022 Blake W. Felt & Angus Gratton
>>>>>>>> v1.20.0:shared/tinyusb/mp_usbd.h
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

<<<<<<<< HEAD:shared-bindings/_bleio/ScanResults.h
#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SCANRESULTS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SCANRESULTS_H

#include "py/obj.h"
#include "shared-module/_bleio/ScanResults.h"

extern const mp_obj_type_t bleio_scanresults_type;

mp_obj_t common_hal_bleio_scanresults_next(bleio_scanresults_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_SCANRESULTS_H
========
#ifndef MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_H
#define MICROPY_INCLUDED_SHARED_TINYUSB_MP_USBD_H

#include "py/obj.h"

// Call instead of tud_task()
void mp_usbd_task(void);

// Function to be implemented in port code.
// Can write a string up to MICROPY_HW_USB_DESC_STR_MAX characters long, plus terminating byte.
extern void mp_usbd_port_get_serial_number(char *buf);

#endif // MICROPY_INCLUDED_SHARED_TINYUSB_USBD_H
>>>>>>>> v1.20.0:shared/tinyusb/mp_usbd.h
