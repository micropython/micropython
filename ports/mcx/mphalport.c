/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 NXP
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

#include "board.h"

extern volatile mp_uint_t s_current_tick;

void mp_hal_init(void) {
    SysTick_Config(CLOCK_GetCoreSysClkFreq() / MICROPY_HAL_SYSTICK_RATE);

    s_current_tick = 0ULL;

    NVIC_SetPriority(SysTick_IRQn, 0U);
    EnableIRQ(SysTick_IRQn);
}

void mp_hal_deinit(void) {
    DisableIRQ(SysTick_IRQn);
    SysTick->CTRL = 0U;
}

void mp_hal_get_unique_id(uint8_t *uid) {
    uint64_t uuid;

    MCX_BoardGetUniqueID(&uuid);

    for (uint8_t i = 0; i < 8; i++) {
        uid[i] = (uuid >> (i * 8)) & 0xFFU;
    }
}
