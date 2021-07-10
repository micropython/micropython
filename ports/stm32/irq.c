/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/obj.h"
#include "py/mphal.h"
#include "irq.h"
#include "modmachine.h"

#if IRQ_ENABLE_STATS
uint32_t irq_stats[IRQ_STATS_MAX] = {0};
#endif

// disable_irq()
// Disable interrupt requests.
// Returns the previous IRQ state which can be passed to enable_irq.
STATIC mp_obj_t machine_disable_irq(void) {
    return mp_obj_new_bool(disable_irq() == IRQ_STATE_ENABLED);
}
MP_DEFINE_CONST_FUN_OBJ_0(machine_disable_irq_obj, machine_disable_irq);

// enable_irq(state=True)
// Enable interrupt requests, based on the argument, which is usually the
// value returned by a previous call to disable_irq.
STATIC mp_obj_t machine_enable_irq(uint n_args, const mp_obj_t *arg) {
    enable_irq((n_args == 0 || mp_obj_is_true(arg[0])) ? IRQ_STATE_ENABLED : IRQ_STATE_DISABLED);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_enable_irq_obj, 0, 1, machine_enable_irq);

#if IRQ_ENABLE_STATS
// return a memoryview of the irq statistics array
STATIC mp_obj_t pyb_irq_stats(void) {
    return mp_obj_new_memoryview(0x80 | 'I', MP_ARRAY_SIZE(irq_stats), &irq_stats[0]);
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_irq_stats_obj, pyb_irq_stats);
#endif
