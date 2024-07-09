// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/I2C.h"

#include "py/mperrno.h"
#include "py/runtime.h"

#define NO_PIN 0xff

void common_hal_busio_i2c_construct(busio_i2c_obj_t *self,
    const mcu_pin_obj_t *scl, const mcu_pin_obj_t *sda, uint32_t frequency, uint32_t timeout) {
    mp_raise_NotImplementedError(NULL);
}

bool common_hal_busio_i2c_deinited(busio_i2c_obj_t *self) {
    return true;
}

void common_hal_busio_i2c_deinit(busio_i2c_obj_t *self) {
    if (common_hal_busio_i2c_deinited(self)) {
        return;
    }
}

bool common_hal_busio_i2c_probe(busio_i2c_obj_t *self, uint8_t addr) {
    return false;
}

bool common_hal_busio_i2c_try_lock(busio_i2c_obj_t *self) {
    return false;
}

bool common_hal_busio_i2c_has_lock(busio_i2c_obj_t *self) {
    return false;
}

void common_hal_busio_i2c_unlock(busio_i2c_obj_t *self) {
}

uint8_t common_hal_busio_i2c_write(busio_i2c_obj_t *self, uint16_t addr,
    const uint8_t *data, size_t len) {
    return 0;
}

uint8_t common_hal_busio_i2c_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *data, size_t len) {
    return MP_EIO;
}

uint8_t common_hal_busio_i2c_write_read(busio_i2c_obj_t *self, uint16_t addr,
    uint8_t *out_data, size_t out_len, uint8_t *in_data, size_t in_len) {
    return MP_EIO;
}

void common_hal_busio_i2c_never_reset(busio_i2c_obj_t *self) {
}
