// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "common-hal/digitalio/DigitalInOut.h"

extern void common_hal_neopixel_write(const digitalio_digitalinout_obj_t *gpio, uint8_t *pixels, uint32_t numBytes);
