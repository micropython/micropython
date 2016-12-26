/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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

#include <stdio.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "timer.h"
#include "hal_timer.h"

#if MICROPY_PY_MACHINE_TIMER

typedef struct _machine_timer_obj_t {
    mp_obj_base_t base;
    Timer_HandleTypeDef *timer;
} machine_timer_obj_t;

Timer_HandleTypeDef TimerHandle0 = {.instance = NULL};
Timer_HandleTypeDef TimerHandle1 = {.instance = NULL};
Timer_HandleTypeDef TimerHandle2 = {.instance = NULL};

STATIC const machine_timer_obj_t machine_timer_obj[] = {
    {{&machine_timer_type}, &TimerHandle0},
    {{&machine_timer_type}, &TimerHandle1},
    {{&machine_timer_type}, &TimerHandle2},
};

void timer_init0(void) {
    // reset the Timer handles
    memset(&TimerHandle0, 0, sizeof(Timer_HandleTypeDef));
    TimerHandle0.instance = TIMER0;
    memset(&TimerHandle1, 0, sizeof(Timer_HandleTypeDef));
    TimerHandle0.instance = TIMER1;
    memset(&TimerHandle2, 0, sizeof(Timer_HandleTypeDef));
    TimerHandle0.instance = TIMER2;
}

#if 0
STATIC int timer_find(mp_obj_t id) {
    // given an integer id
    int timer_id = mp_obj_get_int(id);
    if (timer_id >= 0 && timer_id <= MP_ARRAY_SIZE(machine_timer_obj)
        && machine_timer_obj[timer_id].timer != NULL) {
        return timer_id;
    }
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
        "Timer(%d) does not exist", timer_id));
}
#endif

const mp_obj_type_t machine_timer_type = {
    { &mp_type_type },
    .name = MP_QSTR_Timer,
#if 0
    .print = machine_timer_print,
    .make_new = machine_timer_make_new,
    .locals_dict = (mp_obj_t)&machine_timer_locals_dict
#endif
};

#endif // MICROPY_PY_MACHINE_TIMER
