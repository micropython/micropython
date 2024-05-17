// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_ONEWIREIO_ONEWIRE_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_ONEWIREIO_ONEWIRE_H

#include "common-hal/microcontroller/Pin.h"
#include "shared-module/onewireio/OneWire.h"

extern const mp_obj_type_t onewireio_onewire_type;

extern void common_hal_onewireio_onewire_construct(onewireio_onewire_obj_t *self,
    const mcu_pin_obj_t *pin);
extern void common_hal_onewireio_onewire_deinit(onewireio_onewire_obj_t *self);
extern bool common_hal_onewireio_onewire_deinited(onewireio_onewire_obj_t *self);
extern bool common_hal_onewireio_onewire_reset(onewireio_onewire_obj_t *self);
extern bool common_hal_onewireio_onewire_read_bit(onewireio_onewire_obj_t *self);
extern void common_hal_onewireio_onewire_write_bit(onewireio_onewire_obj_t *self, bool bit);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_ONEWIREIO_ONEWIRE_H
