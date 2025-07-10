/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include <stdint.h>

#include "mpstate.h"
#include "gc.h"
#include "mpthread.h"
#include "gccollect.h"

#if defined(PLAT_RDA)
unsigned int ReadSP(void)
{
    uint32_t res;
    __asm volatile (
        "move %0, $29\n"
        :"=r"(res)
        :
        :
    );
    
    return res;
}
#elif defined(PLAT_ECR6600)

#else
unsigned int ReadSP(void)
{
    uint32_t res;
    __asm volatile (
        "mov %0, r13\n"
        :"=r"(res)
        :
        :
    );
    
    return res;
}
#endif

void gc_stacktop_set(void * ptr)
{
    MP_STATE_PORT(global_stacktop_ptr) = ptr;
}

void gc_collect(void) {
    // get current time, in case we want to time the GC
    #if 0
    uint32_t start = mp_hal_ticks_us();
    #endif
#if defined(PLAT_ECR6600)
	int val = 0;
#endif
    // start the GC
    gc_collect_start();
#if defined(PLAT_ECR6600)
	uintptr_t sp = (uintptr_t)&val;
#else
    // get the registers and the sp
    uintptr_t sp = (uintptr_t)ReadSP();
#endif
    if(mp_is_python_thread())
    {
        // trace the stack, including the registers (since they live on the stack in this function)
        gc_collect_root((void **)sp, ((uint32_t)MP_STATE_THREAD(stack_top) - sp) / sizeof(uint32_t));
    }
    else if(NULL != MP_STATE_PORT(global_stacktop_ptr))
    {
        gc_collect_root((void **)sp, ((uint32_t)MP_STATE_PORT(global_stacktop_ptr) - sp) / sizeof(uint32_t));
    }
    else
    {
        //do nothing
    }
    
    // trace root pointers from any threads
    #if MICROPY_PY_THREAD
    mp_thread_gc_others();
    #endif

    // end the GC
    gc_collect_end();

    #if 0
    // print GC info
    uint32_t ticks = mp_hal_ticks_us() - start;
    gc_info_t info;
    gc_info(&info);
    printf("GC@%lu %lums\n", start, ticks);
    printf(" " UINT_FMT " total\n", info.total);
    printf(" " UINT_FMT " : " UINT_FMT "\n", info.used, info.free);
    printf(" 1=" UINT_FMT " 2=" UINT_FMT " m=" UINT_FMT "\n", info.num_1block, info.num_2block, info.max_block);
    #endif
}

MP_REGISTER_ROOT_POINTER(volatile void * global_stacktop_ptr);
