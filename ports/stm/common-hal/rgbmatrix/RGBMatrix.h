// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#ifndef MICROPY_INCLUDED_STM_COMMON_HAL_RGBMATRIX_RGBMATRIX_H
#define MICROPY_INCLUDED_STM_COMMON_HAL_RGBMATRIX_RGBMATRIX_H

#include "shared-module/rgbmatrix/RGBMatrix.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self);
void common_hal_rgbmatrix_timer_enable(void *);
void common_hal_rgbmatrix_timer_disable(void *);
void common_hal_rgbmatrix_timer_free(void *);

#endif
