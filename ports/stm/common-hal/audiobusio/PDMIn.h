/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 Matthew McGowan for Blues Inc.
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

#ifndef MICROPY_INCLUDED_STM_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H
#define MICROPY_INCLUDED_STM_COMMON_HAL_AUDIOBUSIO_AUDIOOUT_H

#include <stdint.h>
#include "py/obj.h"
#include "peripherals/pins.h"

typedef struct MemsAudio_t MemsAudio;
typedef struct MemsAudio_STM32L4SAIPDM_t MemsAudio_STM32L4SAIPDM;

typedef struct {
    mp_obj_base_t base;
    const mcu_pin_obj_t *clock_pin;
    const mcu_pin_obj_t *data_pin;
    uint32_t sample_rate;
    uint8_t bit_depth;
    bool mono;
    uint8_t oversample;
    MemsAudio *audio;
    MemsAudio_STM32L4SAIPDM *audio_impl;
    /**
     * @brief Flag to indicate from the ISR that recording is complete.
     */
    volatile bool recording_complete;
} audiobusio_pdmin_obj_t;


#endif
