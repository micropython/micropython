// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/busio/SPI.h"

mp_obj_t common_hal_busio_spi_construct_from_device(busio_spi_obj_t *self, const struct device *spi_device) {
    self->base.type = &busio_spi_type;
    common_hal_async_spi_spi_construct_from_device(&self->async_spi, spi_device);
    self->sent = NULL;
    return MP_OBJ_FROM_PTR(self);
}
