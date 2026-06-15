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
#include "machine_tcpwm.h"

#define MACHINE_TCPWM0_NUM_COUNTERS (280U)

static mp_obj_t machine_tcpwm0_counter_owner[MACHINE_TCPWM0_NUM_COUNTERS] = { MP_OBJ_NULL };

void machine_tcpwm_counter_alloc(uint32_t counter_num, mp_obj_t owner) {
    if (counter_num >= MACHINE_TCPWM0_NUM_COUNTERS) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("TCPWM0 counter %lu is not supported"), counter_num);
    }
    if (machine_tcpwm0_counter_owner[counter_num] != MP_OBJ_NULL) {
        mp_raise_msg_varg(&mp_type_ValueError,
            MP_ERROR_TEXT("TCPWM0 counter %lu is already in use by another Timer or PWM instance"), counter_num);
    }
    machine_tcpwm0_counter_owner[counter_num] = owner;
}

void machine_tcpwm_counter_free(uint32_t counter_num, mp_obj_t owner) {
    if (counter_num >= MACHINE_TCPWM0_NUM_COUNTERS) {
        return;
    }
    if (machine_tcpwm0_counter_owner[counter_num] == owner) {
        machine_tcpwm0_counter_owner[counter_num] = MP_OBJ_NULL;
    }
}

en_clk_dst_t machine_tcpwm_counter_pclk(uint32_t counter_num) {
    switch (counter_num) {
        case 0:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN0;
        case 1:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN1;
        case 2:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN2;
        case 3:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN3;
        case 4:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN4;
        case 5:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN5;
        case 6:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN6;
        case 7:
            return PCLK_TCPWM0_CLOCK_COUNTER_EN7;
        default:
            mp_raise_msg_varg(&mp_type_ValueError,
                MP_ERROR_TEXT("TCPWM0 counter %lu is not supported for PWM"), counter_num);
    }
}
