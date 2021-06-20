/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_RP2PIO_STATEMACHINE_H
#define MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_RP2PIO_STATEMACHINE_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "src/rp2_common/hardware_pio/include/hardware/pio.h"

typedef struct {
    mp_obj_base_t base;
    uint32_t pins; // Bitmask of what pins this state machine uses.
    int state_machine;
    PIO pio;
    const uint16_t *init;
    size_t init_len;
    uint32_t initial_pin_state;
    uint32_t initial_pin_direction;
    uint32_t pull_pin_up;
    uint32_t pull_pin_down;
    bool in;
    bool out;
    bool wait_for_txstall;
    uint tx_dreq;
    uint rx_dreq;
    bool out_shift_right;
    bool in_shift_right;
    uint32_t actual_frequency;
    pio_sm_config sm_config;
    uint8_t offset;
} rp2pio_statemachine_obj_t;

void reset_rp2pio_statemachine(void);

// Minimal internal version that only fails on pin error (not in use) or full PIO.
bool rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count,
    uint32_t pull_pin_up, uint32_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count,
    uint32_t initial_pin_state, uint32_t initial_pin_direction,
    uint32_t pins_we_use, bool tx_fifo, bool rx_fifo,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right,
    bool claim_pins);

uint8_t rp2pio_statemachine_program_offset(rp2pio_statemachine_obj_t *self);
void rp2pio_statemachine_set_wrap(rp2pio_statemachine_obj_t *self, uint wrap_target, uint wrap);

void rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self, bool leave_pins);

extern const mp_obj_type_t rp2pio_statemachine_type;

#endif // MICROPY_INCLUDED_RASPBERRYPI_COMMON_HAL_RP2PIO_STATEMACHINE_H
