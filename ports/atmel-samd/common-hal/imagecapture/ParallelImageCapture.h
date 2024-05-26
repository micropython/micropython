// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "shared-bindings/imagecapture/ParallelImageCapture.h"

struct imagecapture_parallelimagecapture_obj {
    mp_obj_base_t base;
    uint8_t data_pin, data_clock, vertical_sync, horizontal_reference, data_count;
};
