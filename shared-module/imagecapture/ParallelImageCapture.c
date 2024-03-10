/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

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
