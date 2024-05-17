// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_USB_HOST_PORT_H
#define MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_USB_HOST_PORT_H

#include "py/obj.h"

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *dp;
    const mcu_pin_obj_t *dm;
} usb_host_port_obj_t;

#endif // MICROPY_INCLUDED_MIMXRT10XX_COMMON_HAL_USB_HOST_PORT_H
