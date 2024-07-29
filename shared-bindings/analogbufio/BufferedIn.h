// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Lee Atkinson, MeanStride Technology, Inc.
//
// SPDX-License-Identifier: MIT

#pragma once

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/analogbufio/BufferedIn.h"

extern const mp_obj_type_t analogbufio_bufferedin_type;

void common_hal_analogbufio_bufferedin_construct(analogbufio_bufferedin_obj_t *self, const mcu_pin_obj_t *pin, uint32_t sample_rate);
void common_hal_analogbufio_bufferedin_deinit(analogbufio_bufferedin_obj_t *self);
bool common_hal_analogbufio_bufferedin_deinited(analogbufio_bufferedin_obj_t *self);
uint32_t common_hal_analogbufio_bufferedin_readinto(analogbufio_bufferedin_obj_t *self, uint8_t *buffer, uint32_t len, uint8_t bytes_per_sample, bool loop);
