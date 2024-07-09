// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/audiobusio/PDMIn.h"
#include "common-hal/microcontroller/Pin.h"
#include "extmod/vfs_fat.h"

extern const mp_obj_type_t audiobusio_pdmin_type;

#if CIRCUITPY_AUDIOBUSIO_PDMIN
void common_hal_audiobusio_pdmin_construct(audiobusio_pdmin_obj_t *self,
    const mcu_pin_obj_t *clock_pin, const mcu_pin_obj_t *data_pin,
    uint32_t sample_rate, uint8_t bit_depth, bool mono, uint8_t oversample);
void common_hal_audiobusio_pdmin_deinit(audiobusio_pdmin_obj_t *self);
bool common_hal_audiobusio_pdmin_deinited(audiobusio_pdmin_obj_t *self);
uint32_t common_hal_audiobusio_pdmin_record_to_buffer(audiobusio_pdmin_obj_t *self,
    uint16_t *buffer, uint32_t length);
uint8_t common_hal_audiobusio_pdmin_get_bit_depth(audiobusio_pdmin_obj_t *self);
uint32_t common_hal_audiobusio_pdmin_get_sample_rate(audiobusio_pdmin_obj_t *self);
// TODO(tannewt): Add record to file
#endif
