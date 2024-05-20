// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Radomir Dopieralski
//
// SPDX-License-Identifier: MIT

#pragma once

#include <stdint.h>
#include "shared-bindings/digitalio/DigitalInOut.h"

typedef struct {
    mp_obj_base_t base;
    uint8_t *buffer;
    mp_obj_t *rows;
    mp_obj_t *cols;
    digitalio_digitalinout_obj_t *buttons;
    uint8_t rows_size;
    uint8_t cols_size;
    uint8_t pressed;
} pew_obj_t;

void pew_init(void);
void pewpew_interrupt_handler(uint8_t index);
void pew_reset(void);
uint16_t pew_get_ticks(void);
