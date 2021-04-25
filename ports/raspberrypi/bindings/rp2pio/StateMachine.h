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

#ifndef MICROPY_INCLUDED_RASPBERRYPI_BINDINGS_RP2PIO_STATEMACHINE_H
#define MICROPY_INCLUDED_RASPBERRYPI_BINDINGS_RP2PIO_STATEMACHINE_H

#include "py/obj.h"

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/rp2pio/StateMachine.h"

// Type object used in Python. Should be shared between ports.
extern const mp_obj_type_t rp2pio_statemachine_type;

// Construct an underlying SPI object.
void common_hal_rp2pio_statemachine_construct(rp2pio_statemachine_obj_t *self,
    const uint16_t *program, size_t program_len,
    size_t frequency,
    const uint16_t *init, size_t init_len,
    const mcu_pin_obj_t *first_out_pin, uint8_t out_pin_count, uint32_t initial_out_pin_state, uint32_t initial_out_pin_direction,
    const mcu_pin_obj_t *first_in_pin, uint8_t in_pin_count, uint32_t pull_pin_up, uint32_t pull_pin_down,
    const mcu_pin_obj_t *first_set_pin, uint8_t set_pin_count, uint32_t initial_set_pin_state, uint32_t initial_set_pin_direction,
    const mcu_pin_obj_t *first_sideset_pin, uint8_t sideset_pin_count, uint32_t initial_sideset_pin_state, uint32_t initial_sideset_pin_direction,
    uint32_t wait_gpio_mask,
    bool exclusive_pin_use,
    bool auto_pull, uint8_t pull_threshold, bool out_shift_right,
    bool wait_for_txstall,
    bool auto_push, uint8_t push_threshold, bool in_shift_right);

void common_hal_rp2pio_statemachine_deinit(rp2pio_statemachine_obj_t *self);
bool common_hal_rp2pio_statemachine_deinited(rp2pio_statemachine_obj_t *self);

void common_hal_rp2pio_statemachine_never_reset(rp2pio_statemachine_obj_t *self);

void common_hal_rp2pio_statemachine_restart(rp2pio_statemachine_obj_t *self);
void common_hal_rp2pio_statemachine_stop(rp2pio_statemachine_obj_t *self);
void common_hal_rp2pio_statemachine_run(rp2pio_statemachine_obj_t *self, const uint16_t *instructions, size_t len);

// Writes out the given data.
bool common_hal_rp2pio_statemachine_write(rp2pio_statemachine_obj_t *self, const uint8_t *data, size_t len, uint8_t stride_in_bytes);
bool common_hal_rp2pio_statemachine_readinto(rp2pio_statemachine_obj_t *self, uint8_t *data, size_t len, uint8_t stride_in_bytes);
bool common_hal_rp2pio_statemachine_write_readinto(rp2pio_statemachine_obj_t *self,
    const uint8_t *data_out, size_t out_len, uint8_t out_stride_in_bytes,
    uint8_t *data_in, size_t in_len, uint8_t in_stride_in_bytes);

// Return actual state machine frequency.
uint32_t common_hal_rp2pio_statemachine_get_frequency(rp2pio_statemachine_obj_t *self);
void common_hal_rp2pio_statemachine_set_frequency(rp2pio_statemachine_obj_t *self, uint32_t frequency);

bool common_hal_rp2pio_statemachine_get_rxstall(rp2pio_statemachine_obj_t *self);
void common_hal_rp2pio_statemachine_clear_rxfifo(rp2pio_statemachine_obj_t *self);
size_t common_hal_rp2pio_statemachine_get_in_waiting(rp2pio_statemachine_obj_t *self);

void common_hal_rp2pio_statemachine_set_interrupt_handler(rp2pio_statemachine_obj_t *self, void (*handler)(void *), void *arg, int mask);

#endif // MICROPY_INCLUDED_RASPBERRYPI_BINDINGS_RP2PIO_STATEMACHINE_H
