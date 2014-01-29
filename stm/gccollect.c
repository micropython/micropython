#include <stdio.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "gc.h"
#include "gccollect.h"
#include "systick.h"

void gc_helper_get_regs_and_clean_stack(machine_uint_t *regs, machine_uint_t heap_end);

void gc_collect(void) {
    uint32_t start = sys_tick_counter;
    gc_collect_start();
    gc_collect_root((void**)&_ram_start, (&_heap_start - &_ram_start) / 4);
    machine_uint_t regs[10];
    gc_helper_get_regs_and_clean_stack(regs, (machine_uint_t)&_heap_end);
    gc_collect_root((void**)&_heap_end, (&_ram_end - &_heap_end) / 4); // will trace regs since they now live in this function on the stack
    gc_collect_end();
    uint32_t ticks = sys_tick_counter - start; // TODO implement a function that does this properly

    if (0) {
        // print GC info
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
