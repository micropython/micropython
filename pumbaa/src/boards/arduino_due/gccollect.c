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
    mp_uint_t regs[10];
    mp_uint_t sp = gc_helper_get_regs_and_sp(regs);
    
    /* Trace the stack, including the registers (since they live on
       the stack in this function). */
    gc_collect_root((void**)sp,
                    ((uint32_t)stack_top_p - sp) / sizeof(uint32_t));

    /* Garbage collect other threads. */
    mp_thread_gc_others();

    /* End the GC. */
    gc_collect_end();
}
