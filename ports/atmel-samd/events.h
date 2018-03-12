/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_ATMEL_SAMD_EVENTS_H
#define MICROPY_INCLUDED_ATMEL_SAMD_EVENTS_H

#include <stdbool.h>
#include <stdint.h>

#include "include/sam.h"

void turn_on_event_system(void);
void reset_event_system(void);
uint8_t find_async_event_channel(void);
uint8_t find_sync_event_channel(void);
void disable_event_channel(uint8_t channel_number);
void disable_event_user(uint8_t user_number);
void connect_event_user_to_channel(uint8_t user, uint8_t channel);
void init_async_event_channel(uint8_t channel, uint8_t generator);
void init_event_channel_interrupt(uint8_t channel, uint8_t gclk, uint8_t generator);
bool event_interrupt_active(uint8_t channel);

#endif  // MICROPY_INCLUDED_ATMEL_SAMD_EVENTS_H
