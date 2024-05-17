// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/paralleldisplaybus/ParallelBus.h"
#include "py/runtime.h"

// If non-sequential pins aren't supported, then this default (weak)
// implementation will raise an exception for you.
MP_WEAK void common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(paralleldisplaybus_parallelbus_obj_t *self,
    uint8_t n_pins, const mcu_pin_obj_t **data_pins, const mcu_pin_obj_t *command, const mcu_pin_obj_t *chip_select,
    const mcu_pin_obj_t *write, const mcu_pin_obj_t *read, const mcu_pin_obj_t *reset, uint32_t frequency) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("This microcontroller only supports data0=, not data_pins=, because it requires contiguous pins."));
}

MP_WEAK void common_hal_paralleldisplaybus_parallelbus_collect_ptrs(mp_obj_t self) {

}
