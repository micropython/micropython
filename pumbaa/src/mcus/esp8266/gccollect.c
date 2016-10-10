/**
 * @file gccollect.c
 *
 * @section License
 * Copyright (C) 2016, Erik Moqvist
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * This file is part of the Pumbaa project.
 */

#include "pumbaa.h"

char *stack_top_p;

extern mp_uint_t gc_helper_get_regs_and_sp(mp_uint_t *regs);

void gc_collect(void)
{
    /* Start the GC. */
    gc_collect_start();
    
    /* Get the registers and the sp */
    mp_uint_t regs[8];
    mp_uint_t sp = gc_helper_get_regs_and_sp(regs);
    
    /* Trace the stack, including the registers (since they live on
       the stack in this function). */
    gc_collect_root((void**)sp,
                    ((uint32_t)stack_top_p - sp) / sizeof(uint32_t));
    
    /* End the GC. */
    gc_collect_end();
}


int *__errno() {
    return NULL;
}

void __assert_func(const char *file,
                   int line,
                   const char *func,
                   const char *expr)
{
    std_printf(FSTR("assert:%s:%d:%s: %s\n"), file, line, func, expr);
    nlr_raise(mp_obj_new_exception_msg(&mp_type_AssertionError,
                                       "C-level assert"));
}
