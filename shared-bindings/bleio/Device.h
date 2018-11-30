/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Artur Pacholec
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DEVICE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DEVICE_H

#include "shared-module/bleio/AdvertisementData.h"
#include "shared-module/bleio/Device.h"
#include "shared-module/bleio/Service.h"

extern const mp_obj_type_t bleio_device_type;

extern void common_hal_bleio_device_add_service(bleio_device_obj_t *device, bleio_service_obj_t *service);
extern void common_hal_bleio_device_start_advertising(bleio_device_obj_t *device, bool connectable, mp_buffer_info_t *raw_data);
extern void common_hal_bleio_device_stop_advertising(bleio_device_obj_t *device);
extern void common_hal_bleio_device_connect(bleio_device_obj_t *device);
extern void common_hal_bleio_device_disconnect(bleio_device_obj_t *device);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_BLEIO_DEVICE_H
