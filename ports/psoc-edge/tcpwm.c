/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Infineon Technologies AG
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

#include "py/runtime.h"
#include "tcpwm.h"

static mp_obj_t machine_tcpwm0_counter[MICROPY_PY_MACHINE_TCPWM0_NUM_COUNTERS] = { MP_OBJ_NULL };

// Helper function to validate if a counter is supported
static bool machine_tcpwm_counter_is_valid(uint32_t counter_num) {
    return (counter_num <= 7) || (counter_num >= 256 && counter_num <= 279);
}

// Helper function to map counter number to array index
// Counters 0-7 map to indices 0-7
// Counters 256-279 map to indices 8-31
static uint32_t machine_tcpwm_counter_to_index(uint32_t counter_num) {
    if (counter_num <= 7) {
        return counter_num;
    } else {
        return counter_num - 256 + 8;
    }
}

bool machine_tcpwm_counter_try_alloc(uint32_t counter_num, mp_obj_t owner) {
    if (!machine_tcpwm_counter_is_valid(counter_num)) {
        return false;
    }
    uint32_t idx = machine_tcpwm_counter_to_index(counter_num);
    if (machine_tcpwm0_counter[idx] != MP_OBJ_NULL) {
        return false;
    }
    machine_tcpwm0_counter[idx] = owner;
    return true;
}

void machine_tcpwm_counter_alloc(uint32_t counter_num, mp_obj_t owner) {
    if (!machine_tcpwm_counter_is_valid(counter_num)) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("TCPWM0 counter %lu is not supported"), counter_num);
    }
    if (!machine_tcpwm_counter_try_alloc(counter_num, owner)) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("TCPWM0 counter %lu is already in use by another Timer or PWM instance"), counter_num);
    }
}

void machine_tcpwm_counter_free(uint32_t counter_num, mp_obj_t owner) {
    if (!machine_tcpwm_counter_is_valid(counter_num)) {
        return;
    }
    uint32_t idx = machine_tcpwm_counter_to_index(counter_num);
    if (machine_tcpwm0_counter[idx] == owner) {
        machine_tcpwm0_counter[idx] = MP_OBJ_NULL;
    }
}

en_clk_dst_t machine_tcpwm_counter_pclk(uint32_t counter_num) {
#define TCPWM_PCLK_CASE(id, counter, irq, pclk_dst) case counter: \
        return pclk_dst;
    switch (counter_num) {
        MICROPY_PY_MACHINE_TCPWM_HW_MAP(TCPWM_PCLK_CASE)
        default:
            mp_raise_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("TCPWM0 counter %lu is not supported"), counter_num);
    }
#undef TCPWM_PCLK_CASE
}
