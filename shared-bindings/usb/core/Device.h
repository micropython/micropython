// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/objarray.h"

#include "shared-module/usb/core/Device.h"

extern const mp_obj_type_t usb_core_device_type;

bool common_hal_usb_core_device_construct(usb_core_device_obj_t *self, uint8_t device_number);
uint16_t common_hal_usb_core_device_get_idVendor(usb_core_device_obj_t *self);
uint16_t common_hal_usb_core_device_get_idProduct(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_serial_number(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_product(usb_core_device_obj_t *self);
mp_obj_t common_hal_usb_core_device_get_manufacturer(usb_core_device_obj_t *self);
void common_hal_usb_core_device_set_configuration(usb_core_device_obj_t *self, mp_int_t configuration);
mp_int_t common_hal_usb_core_device_write(usb_core_device_obj_t *self, mp_int_t endpoint, const uint8_t *buffer, mp_int_t len, mp_int_t timeout);
mp_int_t common_hal_usb_core_device_read(usb_core_device_obj_t *self, mp_int_t endpoint, uint8_t *buffer, mp_int_t len, mp_int_t timeout);
mp_int_t common_hal_usb_core_device_ctrl_transfer(usb_core_device_obj_t *self,
    mp_int_t bmRequestType, mp_int_t bRequest,
    mp_int_t wValue, mp_int_t wIndex,
    uint8_t *buffer, mp_int_t len, mp_int_t timeout);

bool common_hal_usb_core_device_is_kernel_driver_active(usb_core_device_obj_t *self, mp_int_t interface);
void common_hal_usb_core_device_detach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface);
void common_hal_usb_core_device_attach_kernel_driver(usb_core_device_obj_t *self, mp_int_t interface);
