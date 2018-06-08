/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Tobias Badertscher
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
#include <stdarg.h>


#include "py/runtime.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "pyb_irq.h"


void irq_dispatch(pyb_irq_obj_t *self) {
    if (self->handler != mp_const_none) {
        mp_sched_lock();
        // When executing code within a handler we must lock the GC to prevent
        // any memory allocations.  We must also catch any exceptions.
        gc_lock();
        nlr_buf_t nlr;
        if (nlr_push(&nlr) == 0) {//mp_obj_t
            mp_call_function_1(self->handler, (mp_obj_t*)self->base.type);
            nlr_pop();
        } else {
            // Uncaught exception; disable the callback so it doesn't run again.
            self->handler = mp_const_none;
            printf("uncaught exception in %s interrupt handler\n", qstr_str(self->base.type->name));
            mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        }
        gc_unlock();
        mp_sched_unlock();
    }
    self->flags = IRQ_FLAG_NONE;
}

STATIC mp_obj_t pyb_irq_call(mp_obj_t self_in, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    pyb_irq_obj_t *self = self_in;
    mp_arg_check_num(n_args, n_kw, 0, 0, false);
    if (self->handler != mp_const_none) {
        mp_call_function_1(self->handler, (mp_obj_t*)self->base.type);
    }
    return mp_const_none;
}

/// \method trigger()
/// Get the enabled IRQ trigger bitmask:
///
///   - Return or-ed bitmask of IRQ_?? values defined in the main class.
///
STATIC mp_obj_t pyb_irq_trigger(mp_obj_t self_in) {
    pyb_irq_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->enable);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_irq_trigger_obj, pyb_irq_trigger);

/// \method irq_flags()
/// Return the reason executing the callback as or-ed ??_IRQ values.
/// Before leaving the IRQ these flags are cleared (see irq_dispatch).
///
STATIC mp_obj_t pyb_irq_flags(mp_obj_t self_in) {
    pyb_irq_obj_t *self = self_in;
    return MP_OBJ_NEW_SMALL_INT(self->flags);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_irq_flags_obj, pyb_irq_flags);

STATIC const mp_rom_map_elem_t pyb_irq_locals_dict_table[] = {
        { MP_ROM_QSTR(MP_QSTR_trigger),  MP_ROM_PTR(&pyb_irq_trigger_obj) },
        { MP_ROM_QSTR(MP_QSTR_flags),  MP_ROM_PTR(&pyb_irq_flags_obj) },
        { MP_ROM_QSTR(MP_QSTR_NO_IRQ), MP_ROM_INT(IRQ_FLAG_NONE) },
};
STATIC MP_DEFINE_CONST_DICT(pyb_irq_locals_dict, pyb_irq_locals_dict_table);

const mp_obj_type_t pyb_irq_type = {
    { &mp_type_type },
    .name = MP_QSTR_IRQ,
    .call = pyb_irq_call,
    .locals_dict = (mp_obj_dict_t*)&pyb_irq_locals_dict,
};
