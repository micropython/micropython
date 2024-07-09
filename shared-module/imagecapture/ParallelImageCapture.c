// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/imagecapture/ParallelImageCapture.h"
#include "py/runtime.h"

// If the continuous-capture mode isn't supported, then this default (weak) implementation will raise exceptions for you
__attribute__((weak))
void common_hal_imagecapture_parallelimagecapture_continuous_capture_start(imagecapture_parallelimagecapture_obj_t *self, mp_obj_t buffer1, mp_obj_t buffer2) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("This microcontroller does not support continuous capture."));
}

__attribute__((weak))
void common_hal_imagecapture_parallelimagecapture_continuous_capture_stop(imagecapture_parallelimagecapture_obj_t *self) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("This microcontroller does not support continuous capture."));
}

__attribute__((weak))
mp_obj_t common_hal_imagecapture_parallelimagecapture_continuous_capture_get_frame(imagecapture_parallelimagecapture_obj_t *self) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("This microcontroller does not support continuous capture."));
}
