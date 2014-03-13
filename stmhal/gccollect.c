#include <stdio.h>

#include <stm32f4xx_hal.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "gc.h"
#include "gccollect.h"

machine_uint_t gc_helper_get_regs_and_sp(machine_uint_t *regs);

// obsolete
// void gc_helper_get_regs_and_clean_stack(machine_uint_t *regs, machine_uint_t heap_end);

void gc_collect(void) {
    // get current time, in case we want to time the GC
    uint32_t start = HAL_GetTick();

    // start the GC
    gc_collect_start();

    // scan everything in RAM before the heap
    // this includes the data and bss segments
    // TODO possibly don't need to scan data, since all pointers should start out NULL and be in bss
    gc_collect_root((void**)&_ram_start, ((uint32_t)&_ebss - (uint32_t)&_ram_start) / sizeof(uint32_t));

    // get the registers and the sp
    machine_uint_t regs[10];
    machine_uint_t sp = gc_helper_get_regs_and_sp(regs);

    // trace the stack, including the registers (since they live on the stack in this function)
    gc_collect_root((void**)sp, ((uint32_t)&_ram_end - sp) / sizeof(uint32_t));

    // end the GC
    gc_collect_end();

    if (0) {
        // print GC info
        uint32_t ticks = HAL_GetTick() - start; // TODO implement a function that does this properly
        gc_info_t info;
        gc_info(&info);
        printf("GC@%lu %lums\n", start, ticks);
        printf(" %lu total\n", info.total);
        printf(" %lu : %lu\n", info.used, info.free);
        printf(" 1=%lu 2=%lu m=%lu\n", info.num_1block, info.num_2block, info.max_block);
    }
}

static mp_obj_t pyb_gc(void) {
    gc_collect();
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_gc_obj, pyb_gc);
