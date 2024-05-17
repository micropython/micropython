// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"

typedef struct imagecapture_parallelimagecapture_obj imagecapture_parallelimagecapture_obj_t;
extern const mp_obj_type_t imagecapture_parallelimagecapture_type;

// if only the first element of data_pins is non-NULL, the pins are sequential in microcontroller pin numbering.
void common_hal_imagecapture_parallelimagecapture_construct(imagecapture_parallelimagecapture_obj_t *self,
    const uint8_t data_pins[],
    uint8_t data_count,
    const mcu_pin_obj_t *data_clock,
    const mcu_pin_obj_t *vertical_sync,
    const mcu_pin_obj_t *horizontal_sync);
void common_hal_imagecapture_parallelimagecapture_deinit(imagecapture_parallelimagecapture_obj_t *self);
bool common_hal_imagecapture_parallelimagecapture_deinited(imagecapture_parallelimagecapture_obj_t *self);
void common_hal_imagecapture_parallelimagecapture_singleshot_capture(imagecapture_parallelimagecapture_obj_t *self, mp_obj_t buffer);
void common_hal_imagecapture_parallelimagecapture_continuous_capture_start(imagecapture_parallelimagecapture_obj_t *self, mp_obj_t buffer1, mp_obj_t buffer2);
void common_hal_imagecapture_parallelimagecapture_continuous_capture_stop(imagecapture_parallelimagecapture_obj_t *self);
mp_obj_t common_hal_imagecapture_parallelimagecapture_continuous_capture_get_frame(imagecapture_parallelimagecapture_obj_t *self);
