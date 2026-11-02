// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

// busio.SPI is built on common-hal/async_spi.
#include "shared-module/busio/SPI.h"

// A fixed devicetree device, for the generated board.SPI().
mp_obj_t common_hal_busio_spi_construct_from_device(busio_spi_obj_t *self, const struct device *spi_device);
