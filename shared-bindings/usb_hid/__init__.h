// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "py/obj.h"
#include "py/objtuple.h"
#include "shared-module/usb_hid/__init__.h"

extern mp_obj_tuple_t common_hal_usb_hid_devices;

void usb_hid_set_devices(mp_obj_t devices);

bool common_hal_usb_hid_disable(void);
bool common_hal_usb_hid_enable(const mp_obj_t devices_seq, uint8_t boot_device);
uint8_t common_hal_usb_hid_get_boot_device(void);
