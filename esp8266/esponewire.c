/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Damien P. George
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

#include <stdint.h>

#include "etshal.h"
#include "user_interface.h"
#include "modpyb.h"
#include "esponewire.h"

#define TIMING_RESET1 (0)
#define TIMING_RESET2 (1)
#define TIMING_RESET3 (2)
#define TIMING_READ1 (3)
#define TIMING_READ2 (4)
#define TIMING_READ3 (5)
#define TIMING_WRITE1 (6)
#define TIMING_WRITE2 (7)
#define TIMING_WRITE3 (8)

uint16_t esp_onewire_timings[9] = {480, 40, 420, 5, 5, 40, 10, 50, 10};

static uint32_t disable_irq(void) {
    ets_intr_lock();
    return 0;
}

static void enable_irq(uint32_t i) {
    ets_intr_unlock();
}

static void mp_hal_delay_us_no_irq(uint32_t us) {
    uint32_t start = system_get_time();
    while (system_get_time() - start < us) {
    }
}

#define DELAY_US mp_hal_delay_us_no_irq

int esp_onewire_reset(uint pin) {
    pin_set(pin, 0);
    DELAY_US(esp_onewire_timings[TIMING_RESET1]);
    uint32_t i = disable_irq();
    pin_set(pin, 1);
    DELAY_US(esp_onewire_timings[TIMING_RESET2]);
    int status = !pin_get(pin);
    enable_irq(i);
    DELAY_US(esp_onewire_timings[TIMING_RESET3]);
    return status;
}

int esp_onewire_readbit(uint pin) {
    pin_set(pin, 1);
    uint32_t i = disable_irq();
    pin_set(pin, 0);
    DELAY_US(esp_onewire_timings[TIMING_READ1]);
    pin_set(pin, 1);
    DELAY_US(esp_onewire_timings[TIMING_READ2]);
    int value = pin_get(pin);
    enable_irq(i);
    DELAY_US(esp_onewire_timings[TIMING_READ3]);
    return value;
}

void esp_onewire_writebit(uint pin, int value) {
    uint32_t i = disable_irq();
    pin_set(pin, 0);
    DELAY_US(esp_onewire_timings[TIMING_WRITE1]);
    if (value) {
        pin_set(pin, 1);
    }
    DELAY_US(esp_onewire_timings[TIMING_WRITE2]);
    pin_set(pin, 1);
    DELAY_US(esp_onewire_timings[TIMING_WRITE3]);
    enable_irq(i);
}
