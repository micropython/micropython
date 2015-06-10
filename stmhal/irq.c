/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "py/nlr.h"
#include "py/obj.h"
#include "py/mphal.h"
#include "irq.h"

/// \moduleref pyb

#if IRQ_ENABLE_STATS
uint32_t irq_stats[FPU_IRQn + 1] = {0};
#endif

/// \function wfi()
/// Wait for an interrupt.
/// This executies a `wfi` instruction which reduces power consumption
/// of the MCU until an interrupt occurs, at which point execution continues.
STATIC mp_obj_t pyb_wfi(void) {
    __WFI();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_wfi_obj, pyb_wfi);

/// \function disable_irq()
/// Disable interrupt requests.
/// Returns the previous IRQ state: `False`/`True` for disabled/enabled IRQs
/// respectively.  This return value can be passed to enable_irq to restore
/// the IRQ to its original state.
STATIC mp_obj_t pyb_disable_irq(void) {
    return mp_obj_new_bool(disable_irq() == IRQ_STATE_ENABLED);
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_disable_irq_obj, pyb_disable_irq);

/// \function enable_irq(state=True)
/// Enable interrupt requests.
/// If `state` is `True` (the default value) then IRQs are enabled.
/// If `state` is `False` then IRQs are disabled.  The most common use of
/// this function is to pass it the value returned by `disable_irq` to
/// exit a critical section.
STATIC mp_obj_t pyb_enable_irq(uint n_args, const mp_obj_t *arg) {
    enable_irq((n_args == 0 || mp_obj_is_true(arg[0])) ? IRQ_STATE_ENABLED : IRQ_STATE_DISABLED);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(pyb_enable_irq_obj, 0, 1, pyb_enable_irq);

#if IRQ_ENABLE_STATS
// return a memoryview of the irq statistics array
STATIC mp_obj_t pyb_irq_stats(void) {
    return mp_obj_new_memoryview(0x80 | 'I', MP_ARRAY_SIZE(irq_stats), &irq_stats[0]);
}
MP_DEFINE_CONST_FUN_OBJ_0(pyb_irq_stats_obj, pyb_irq_stats);
#endif
