// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"

#include "py/runtime.h"

void common_hal_busio_spi_construct(busio_spi_obj_t *self,
    const mcu_pin_obj_t *clock, const mcu_pin_obj_t *mosi,
    const mcu_pin_obj_t *miso, bool half_duplex) {
    mp_raise_NotImplementedError(NULL);
}

void common_hal_busio_spi_never_reset(busio_spi_obj_t *self) {
}

bool common_hal_busio_spi_deinited(busio_spi_obj_t *self) {
    return true;
}

void common_hal_busio_spi_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        return;
    }
}

bool common_hal_busio_spi_configure(busio_spi_obj_t *self,
    uint32_t baudrate, uint8_t polarity, uint8_t phase, uint8_t bits) {
    return true;
}

bool common_hal_busio_spi_try_lock(busio_spi_obj_t *self) {
    return false;
}

bool common_hal_busio_spi_has_lock(busio_spi_obj_t *self) {
    return false;
}

void common_hal_busio_spi_unlock(busio_spi_obj_t *self) {
}


bool common_hal_busio_spi_write(busio_spi_obj_t *self,
    const uint8_t *data, size_t len) {
    return false;
}

bool common_hal_busio_spi_read(busio_spi_obj_t *self,
    uint8_t *data, size_t len, uint8_t write_value) {
    return false;
}

bool common_hal_busio_spi_transfer(busio_spi_obj_t *self, const uint8_t *data_out, uint8_t *data_in, size_t len) {
    return false;
}

uint32_t common_hal_busio_spi_get_frequency(busio_spi_obj_t *self) {
    return 0;
}

uint8_t common_hal_busio_spi_get_phase(busio_spi_obj_t *self) {
    return 0;
}

uint8_t common_hal_busio_spi_get_polarity(busio_spi_obj_t *self) {
    return 0;
}
